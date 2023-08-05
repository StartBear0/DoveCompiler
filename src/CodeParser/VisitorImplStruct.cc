#include <vector>

#include "VisitorImpl.h"

namespace front
{

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
        }
        else if (context->stmt())
        {
            context->stmt()->accept(this);
        }
        return 0;
    };

    std::any VisitorImpl::visitAssignmentStmt(SysYParser::AssignmentStmtContext *context)
    {
        try
        {
            auto left = std::any_cast<std::shared_ptr<ir::User>>(context->lVal()->accept(this));
            auto right = std::any_cast<std::shared_ptr<ir::User>>(context->exp()->accept(this));
            auto store = std::make_shared<ir::Store>(right, left);
            ctx_.currentBasicBlock->addInstruction(store);
            return std::dynamic_pointer_cast<ir::User>(store);
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Error in visitAssignmentStmt:\n" + std::string(e.what()));
        }
    };

    std::any VisitorImpl::visitExpStmt(SysYParser::ExpStmtContext *context)
    {
        try
        {
            return std::any_cast<std::shared_ptr<ir::User>>(context->exp()->accept(this));
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Error in visitExpStmt:\n" + std::string(e.what()));
        }
    };

    std::any VisitorImpl::visitBlockStmt(SysYParser::BlockStmtContext *context)
    {
        // Do nothing, just accept.
        context->block()->accept(this);
        return 0;
    };

    std::any VisitorImpl::visitCond(SysYParser::CondContext *context)
    {
        return context->lOrExp()->accept(this);
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
        auto entry_bb = std::make_shared<ir::BasicBlock>(entry_jplabel, ir::BlockType::WhileEntry);
        auto body_bb = std::make_shared<ir::BasicBlock>(body_jplabel, ir::BlockType::WhileBody);
        auto exit_bb = std::make_shared<ir::BasicBlock>(exit_jplabel, ir::BlockType::Default);
        auto entry_label = std::make_shared<ir::Label>(entry_jplabel);
        auto body_label = std::make_shared<ir::Label>(body_jplabel);
        auto exit_label = std::make_shared<ir::Label>(exit_jplabel);
        auto out_exit_bb = ctx_.exitBasicBlock;

        ctx_.symbolTable->pushScope("while");
        ctx_.continueBBStack.push(entry_bb);
        ctx_.breakBBStack.push(exit_bb);
        ctx_.currentFunction->addBasicBlock(entry_bb);
        ctx_.entryBasicBlock = ctx_.currentBasicBlock;
        ctx_.currentBasicBlock = entry_bb;
        ctx_.exitBasicBlock = exit_bb;
        auto cond = std::any_cast<std::shared_ptr<ir::Value>>(context->cond()->accept(this));
        std::shared_ptr<ir::Br> entry_br;
        if (cond->getValueType() == ir::ValueType::StaticValue && false) //! for debug
        {
            if (cond->getType()->isPrimitive() && std::dynamic_pointer_cast<ir::PrimitiveDataType>(cond->getType())->isBool() && std::dynamic_pointer_cast<ir::StaticValue>(cond)->getBool())
            {
                entry_br = std::make_shared<ir::Br>(body_label, body_label->getName());
            }
            else
            {
                entry_br = std::make_shared<ir::Br>(exit_label, exit_label->getName());
            }
        }
        else
        {
            entry_br = std::make_shared<ir::Br>(cond, body_label, exit_label, entry_label->getName());
        }
        ctx_.currentBasicBlock->addInstruction(entry_br);

        ctx_.currentFunction->addBasicBlock(body_bb);
        ctx_.currentBasicBlock = body_bb;
        context->stmt()->accept(this);
        auto body_br = std::make_shared<ir::Br>(entry_label, entry_label->getName());
        ctx_.currentBasicBlock->addInstruction(body_br);
        ctx_.continueBBStack.pop();
        ctx_.breakBBStack.pop();
        ctx_.symbolTable->popScope();

        ctx_.currentFunction->addBasicBlock(exit_bb);
        ctx_.entryBasicBlock = ctx_.currentBasicBlock;
        ctx_.currentBasicBlock = exit_bb;
        ctx_.exitBasicBlock = out_exit_bb;

        return 0;
    };

    std::any VisitorImpl::visitBreakStmt(SysYParser::BreakStmtContext *context)
    {
        auto label = std::make_shared<ir::Label>(ctx_.breakBBStack.top()->getJPLabel());
        auto br = std::make_shared<ir::Br>(label, label->getName());
        ctx_.currentBasicBlock->addInstruction(br);
        return std::dynamic_pointer_cast<ir::User>(br);
    };

    std::any VisitorImpl::visitContinueStmt(SysYParser::ContinueStmtContext *context)
    {
        auto label = std::make_shared<ir::Label>(ctx_.continueBBStack.top()->getJPLabel());
        auto br = std::make_shared<ir::Br>(label, label->getName());
        ctx_.currentBasicBlock->addInstruction(br);
        return std::dynamic_pointer_cast<ir::User>(br);
    };

    std::any VisitorImpl::visitReturnStmt(SysYParser::ReturnStmtContext *context)
    {
        auto user = std::any_cast<std::shared_ptr<ir::User>>(context->exp()->accept(this));
        auto ret = std::make_shared<ir::Return>(user, user->getName());
        ctx_.currentBasicBlock->addInstruction(ret);
        return std::dynamic_pointer_cast<ir::User>(ret);
    };

}