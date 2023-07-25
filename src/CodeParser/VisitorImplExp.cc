#include <vector>

#include "VisitorImpl.h"
#include "Utils/Utils.h"
namespace front
{

  std::any VisitorImpl::visitExp(SysYParser::ExpContext *context)
  {
    return context->addExp()->accept(this);
  };
  std::any VisitorImpl::visitCond(SysYParser::CondContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitLVal(SysYParser::LValContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitRVal(SysYParser::RValContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitPrimParenExp(SysYParser::PrimParenExpContext *context)
  {
    return context->exp()->accept(this);
  };
  std::any VisitorImpl::visitPrimRvalExp(SysYParser::PrimRvalExpContext *context)
  {
    return context->rVal()->accept(this);
  };
  std::any VisitorImpl::visitPrimConstExp(SysYParser::PrimConstExpContext *context)
  {
    return context->number()->accept(this);
  };
  std::any VisitorImpl::visitNumber(SysYParser::NumberContext *context)
  { // TODO
    auto p = std::shared_ptr<ir::Value>();
    if (context->IntLiteral())
    {
      auto i = utils::ParseInt32(context->getText());
      p = std::make_shared<ir::Constant>(ir::MakePrimitiveDataType(ir::PrimitiveDataType::TypeID::Int32), "Int", i);
      // Constant(std::shared_ptr<Type> type, uint32_t val)
    }
    else if (context->FloatLiteral())
    {
      auto i = utils::ParseFloat32(context->getText());
      p = std::make_shared<ir::Constant>(ir::MakePrimitiveDataType(ir::PrimitiveDataType::TypeID::Float32), "Float", i);
    }
    // std::vector<std::shared_ptr<ir::Allocate>> allocates;

    return p;
  };
  std::any VisitorImpl::visitUnaryToPrimExp(SysYParser::UnaryToPrimExpContext *context)
  {
    return context->primExp()->accept(this);
  };
  std::any VisitorImpl::visitUnaryFuncCallExp(SysYParser::UnaryFuncCallExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitUnaryOpExp(SysYParser::UnaryOpExpContext *context)
  {
    auto unaryOp = std::any_cast<char>(context->unaryOp()->accept(this));
    return 0;
  };
  std::any VisitorImpl::visitUnaryOp(SysYParser::UnaryOpContext *context)
  {
    if (context->Addition())
    {
      return '+';
    }
    else if (context->Minus())
    {
      return '-';
    }
    else if (context->Exclamation())
    {
      return '!';
    }
    return '\0';
  };
  std::any VisitorImpl::visitFuncRparamList(SysYParser::FuncRparamListContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitExpAsRparam(SysYParser::ExpAsRparamContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitStringAsRparam(SysYParser::StringAsRparamContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitMulTwoExp(SysYParser::MulTwoExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitMulToUnaryExp(SysYParser::MulToUnaryExpContext *context)
  {
    return context->unaryExp()->accept(this);
  };
  std::any VisitorImpl::visitAddToMulExp(SysYParser::AddToMulExpContext *context)
  {
    return context->mulExp()->accept(this);
  };
  std::any VisitorImpl::visitAddTwoExp(SysYParser::AddTwoExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitRelToAddExp(SysYParser::RelToAddExpContext *context)
  {
    return context->addExp()->accept(this);
  };
  std::any VisitorImpl::visitRelTwoExp(SysYParser::RelTwoExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitEqTwoExp(SysYParser::EqTwoExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitEqToRelExp(SysYParser::EqToRelExpContext *context)
  {
    return context->relExp()->accept(this);
  };
  std::any VisitorImpl::visitLAndToEqExp(SysYParser::LAndToEqExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitLAndTwoExp(SysYParser::LAndTwoExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitLOrToLandExp(SysYParser::LOrToLandExpContext *context)
  { // TODO
    return 0;
  };
  std::any VisitorImpl::visitLOrTwoExp(SysYParser::LOrTwoExpContext *context)
  { // TODO
    return 0;
  };
  //* @return ir::Constant
  std::any VisitorImpl::visitConstExp(SysYParser::ConstExpContext *context)
  {
    auto constant = std::make_shared<ir::Constant>(ir::MakePrimitiveDataType(ir::PrimitiveDataType::TypeID::Int32), "Int", int32_t(2));
    return constant;
  };
}