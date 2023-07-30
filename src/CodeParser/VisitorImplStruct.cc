#include <vector>

#include "VisitorImpl.h"

namespace front
{
  std::any VisitorImpl::visitFuncDef(SysYParser::FuncDefContext *context)
  {
    auto typeID = std::any_cast<ir::PrimitiveDataType::TypeID>(context->funcType()->accept(this));
    std::string funcName = context->Identifier()->getSymbol()->getText();
    ctx_.currentFunction = std::make_shared<ir::Function>(ir::MakePrimitiveDataType(typeID), funcName);
    ctx_.symbolTable->pushScope("function");

    if (context->block())
    {
      // Add entry basic block to function
      auto entry_bb_name = "_" + funcName + "_entry";
      auto bb = std::make_shared<ir::BasicBlock>(entry_bb_name, ir::BasicBlock::BlockType::FunctionEntry);
      ctx_.currentBasicBlock = bb;
      ctx_.currentFunction->addBasicBlock(bb);

      context->block()->accept(this);
    }

    ctx_.symbolTable->popScope();
    return ctx_.currentFunction;
  };
  std::any VisitorImpl::visitFuncType(SysYParser::FuncTypeContext *context)
  {
    auto typeID = ir::PrimitiveDataType::TypeID::Void;
    if (context->Void())
    {
      typeID = ir::PrimitiveDataType::TypeID::Void;
    }
    else if (context->Int())
    {
      typeID = ir::PrimitiveDataType::TypeID::Int32;
    }
    else if (context->Float())
    {
      typeID = ir::PrimitiveDataType::TypeID::Float32;
    }
    return typeID;
  };
  std::any VisitorImpl::visitFuncFparamList(SysYParser::FuncFparamListContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitFuncFparam(SysYParser::FuncFparamContext *context)
  { // TODO
    return 0;
  };

  std::any VisitorImpl::visitBlock(SysYParser::BlockContext *context)
  {
    // This block is different from the BasicBlock in ControlFlow, this block is a scope
    ctx_.symbolTable->pushScope("block");
    for (auto blockItem : context->blockItem())
    {
      blockItem->accept(this);
    }
    return 0;
  };
  std::any VisitorImpl::visitBlockItem(SysYParser::BlockItemContext *context)
  {
    if (context->decl())
    {
      context->decl()->accept(this);
      // TODO Add decl allocates to current basic block
    }
    else if (context->stmt())
    {
      context->stmt()->accept(this);
    }
    return 0;
  };
  std::any VisitorImpl::visitAssignmentStmt(SysYParser::AssignmentStmtContext *context)
  { // TODO Add this instructions to current basic block
    return 0;
  };
  std::any VisitorImpl::visitExpStmt(SysYParser::ExpStmtContext *context)
  { // TODO Add this instruction to current basic block
    return 0;
  };
  std::any VisitorImpl::visitBlockStmt(SysYParser::BlockStmtContext *context)
  {
    // Do nothing, just accept.
    context->block()->accept(this);
    return 0;
  };
  std::any VisitorImpl::visitIfStmt(SysYParser::IfStmtContext *context)
  {

    auto t_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_if_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto exit_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_if_exit_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto bb = std::make_shared<ir::BasicBlock>(t_jplabel, ir::BasicBlock::BlockType::IfTrue);
    auto exit_bb = std::make_shared<ir::BasicBlock>(exit_jplabel, ir::BasicBlock::BlockType::Default);

    ctx_.symbolTable->pushScope("if");
    ctx_.currentFunction->addBasicBlock(bb);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = bb;
    ctx_.exitBasicBlock = exit_bb;
    context->stmt()->accept(this);
    ctx_.symbolTable->popScope();

    ctx_.currentFunction->addBasicBlock(exit_bb);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = exit_bb;

    return 0;
  };
  std::any VisitorImpl::visitIfElseStmt(SysYParser::IfElseStmtContext *context)
  {

    auto t_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_if_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto f_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_else_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto exit_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_if_exit_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto bbt = std::make_shared<ir::BasicBlock>(t_jplabel, ir::BasicBlock::BlockType::IfTrue);
    auto bbf = std::make_shared<ir::BasicBlock>(f_jplabel, ir::BasicBlock::BlockType::IfFalse);
    auto exit_bb = std::make_shared<ir::BasicBlock>(exit_jplabel, ir::BasicBlock::BlockType::Default);

    ctx_.symbolTable->pushScope("if");
    ctx_.currentFunction->addBasicBlock(bbt);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = bbt;
    context->stmt(0)->accept(this);
    ctx_.symbolTable->popScope();

    ctx_.symbolTable->pushScope("else");
    ctx_.currentFunction->addBasicBlock(bbf);
    ctx_.currentBasicBlock = bbf;
    context->stmt(1)->accept(this);
    ctx_.symbolTable->popScope();

    ctx_.currentFunction->addBasicBlock(exit_bb);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = exit_bb;

    return 0;
  };
  std::any VisitorImpl::visitWhileStmt(SysYParser::WhileStmtContext *context)
  {

    auto entry_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_while_entry_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto body_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_while_body_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto exit_jplabel = std::make_shared<ir::JPLabel>("_" + ctx_.currentFunction->getName() + "_while_exit_" + std::to_string(ctx_.basicBlockCounter.next()));
    auto entry_bb = std::make_shared<ir::BasicBlock>(entry_jplabel, ir::BasicBlock::BlockType::WhileEntry);
    auto body_bb = std::make_shared<ir::BasicBlock>(body_jplabel, ir::BasicBlock::BlockType::WhileBody);
    auto exit_bb = std::make_shared<ir::BasicBlock>(exit_jplabel, ir::BasicBlock::BlockType::Default);

    ctx_.symbolTable->pushScope("while");
    ctx_.continueBBStack.push(entry_bb);
    ctx_.breakBBStack.push(exit_bb);
    ctx_.currentFunction->addBasicBlock(entry_bb);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = entry_bb;
    ctx_.exitBasicBlock = exit_bb;
    context->cond()->accept(this);

    ctx_.currentFunction->addBasicBlock(body_bb);
    ctx_.currentBasicBlock = body_bb;
    context->stmt()->accept(this);
    ctx_.continueBBStack.pop();
    ctx_.breakBBStack.pop();
    ctx_.symbolTable->popScope();

    ctx_.currentFunction->addBasicBlock(exit_bb);
    ctx_.entryBasicBlock = ctx_.currentBasicBlock;
    ctx_.currentBasicBlock = exit_bb;

    return 0;
  };
  std::any VisitorImpl::visitBreakStmt(SysYParser::BreakStmtContext *context)
  {
    auto label = std::make_shared<ir::Label>(ctx_.breakBBStack.top()->getLabel());
    auto br = std::make_shared<ir::Br>(label, label->getName());
    ctx_.currentBasicBlock->addInstruction(br);
    return 0;
  };
  std::any VisitorImpl::visitContinueStmt(SysYParser::ContinueStmtContext *context)
  { // TODO Jump to loop entry basic block
    return 0;
  };
  std::any VisitorImpl::visitReturnStmt(SysYParser::ReturnStmtContext *context)
  { // TODO Just add this instruction to current basic block
    return 0;
  };

}