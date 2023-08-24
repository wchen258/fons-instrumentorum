#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;


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
          fprintf(stderr, "Loop entry point: %d, size %d\nat line number:", block->getBlockID(), block->size());
          SourceLocation sloc = stmt->getBeginLoc();
          unsigned int lino = sm.getSpellingLineNumber(sloc);
          printf("%d\n", lino);
          
        }
      }

      auto langOpt = context->getLangOpts();
      sourceCFG->dump(langOpt, true);
    }
  }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nMore help text...\n");

static cl::opt<std::string> funcname("funcname", cl::desc("Specify the function name to analyze"), cl::value_desc("funcname"));

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
  
  // find all functions inside the main


  Finder.addMatcher(FunctionMatcher, &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
