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
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nMore help text...\n");
static cl::opt<std::string> funcname("funcname", cl::desc("Specify the function name to analyze."), cl::value_desc("funcname"));
static cl::opt<std::string> instrument("instrument", cl::desc("If set, instrument <input string> at each loop line number minus 1."), cl::value_desc("string"));

void InsertCodeAtLine(clang::SourceManager &SM, clang::Rewriter &R, unsigned LineNo, const std::string &Code) {
    clang::FileID MainFile = SM.getMainFileID();
    clang::SourceLocation Loc = SM.translateLineCol(MainFile, LineNo, 1);
    R.InsertText(Loc, Code, true);
}

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
        TheRewriter.getEditBuffer(sm.getMainFileID()).write(llvm::outs());
      }
    }
  }
};

int main(int argc, const char **argv)
{
  Expected<CommonOptionsParser> options = CommonOptionsParser::create(argc, argv, MyToolCategory);
  assert(static_cast<bool>(options));
  ClangTool Tool(options->getCompilations(),
                 options->getSourcePathList());

  CFGPrinter Printer;
  MatchFinder Finder;

  DeclarationMatcher FunctionMatcher = functionDecl(isMain()).bind("targetFunc");
  if (!funcname.empty()){
    FunctionMatcher = functionDecl(hasName(funcname)).bind("targetFunc");
  }
  Finder.addMatcher(FunctionMatcher, &Printer);
  return Tool.run(newFrontendActionFactory(&Finder).get());
}
