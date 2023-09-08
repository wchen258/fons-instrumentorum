#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Core/Rewriter.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static cl::opt<std::string> funcname("funcname", cl::desc("Specify the function name to analyze."), cl::value_desc("function name"));
static cl::opt<std::string> instrument("instrument", cl::desc("If set, instrument <expression> at each loop line number minus 1."), cl::value_desc("expression"));
static cl::opt<std::string> ofname("o", cl::desc("Specify the output file name."), cl::value_desc("filename"));

class CFGPrinter : public MatchFinder::MatchCallback
{
public:
  virtual void run(const MatchFinder::MatchResult &Result)
  {
    if (const FunctionDecl *funcDecl =
            Result.Nodes.getNodeAs<clang::FunctionDecl>("targetFunc"))
    {
      ASTContext *context = Result.Context;
      SourceManager &sm = context->getSourceManager();
      clang::Rewriter TheRewriter;
      TheRewriter.setSourceMgr(sm, context->getLangOpts());

      if (!funcDecl->isThisDeclarationADefinition()) {
        // print the position of the function to stderr
        SourceLocation sloc = funcDecl->getBeginLoc();
        unsigned int lino = sm.getSpellingLineNumber(sloc);
        unsigned int cno = sm.getSpellingColumnNumber(sloc);
        fprintf(stderr, "Function %s is a declaration at (%u,%u).\n", funcDecl->getNameAsString().c_str(), lino, cno);
        return;
      } 


      Stmt *funcBody = funcDecl->getBody();
      static std::unique_ptr<CFG> sourceCFG = CFG::buildCFG(
          funcDecl, funcBody, context, clang::CFG::BuildOptions());

      // find all the natural loops in the CFG, and print the entry point of each loop
      for (auto it = sourceCFG->begin(); it != sourceCFG->end(); ++it)
      {
        CFGBlock *block = *it;
        const Stmt *stmt = block->getLoopTarget();
        if (stmt)
        {
          SourceLocation sloc = stmt->getBeginLoc();
          unsigned int lino = sm.getSpellingLineNumber(sloc);
          unsigned int cno = sm.getSpellingColumnNumber(sloc);
          fprintf(stderr, "Loop entry point (BB id): %d, at line number: %d\n", block->getBlockID(), lino);
          if (!instrument.empty()) {
            SourceLocation loc = sm.translateLineCol(sm.getMainFileID(), lino, cno);
            TheRewriter.InsertText(loc, instrument + "\n", true, true);
          }
        }
      }

      auto langOpt = context->getLangOpts();
      sourceCFG->dump(langOpt, true);
      if (!instrument.empty()) {
        // write the editbuffer to [ofname], if specified
        if (!ofname.empty()) {
          std::error_code error_code;
          llvm::raw_fd_ostream out(ofname, error_code);
          TheRewriter.getEditBuffer(sm.getMainFileID()).write(out);
        } else {
          TheRewriter.getEditBuffer(sm.getMainFileID()).write(llvm::outs());
        }
      }
    }
  }
};

int main(int argc, const char **argv)
{
  // cl::ParseCommandLineOptions(argc, argv);
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  CFGPrinter Printer;
  MatchFinder Finder;

  DeclarationMatcher FunctionMatcher = functionDecl(isMain()).bind("targetFunc");
  if (!funcname.empty()){
    FunctionMatcher = functionDecl(hasName(funcname)).bind("targetFunc");
  }
  Finder.addMatcher(FunctionMatcher, &Printer);
  return Tool.run(newFrontendActionFactory(&Finder).get());
}
