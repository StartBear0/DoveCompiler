#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <string>

#include "Type.h"
#include "Instruction.h"

namespace ir
{
  class Constant : public User
  {
  public:
    explicit Constant(std::shared_ptr<Type> type, int32_t val) : User(type, "ConstantInt"), is_int_(true), int_val_(val) { is_constant_ = true; }
    explicit Constant(std::shared_ptr<Type> type, float val) : User(type, "ConstantFloat"), is_float_(true), float_val_(val) { is_constant_ = true; }
    explicit Constant(std::shared_ptr<Type> type, bool val) : User(type, "ConstantBoolean"), is_bool_(true), bool_val_(val) { is_constant_ = true; }
    std::string dump(DumpHelper &helper) const override
    {
      std::string output = "Constant ";
      if (is_int_)
        output += std::to_string(int_val_);
      else if (is_float_)
        output += std::to_string(float_val_);
      else if (is_bool_)
        output += std::to_string(bool_val_);
      else
        output += "null";
      helper.add(output);
      return output;
    }

    bool isInt() const
    {
      return is_int_;
    }

    bool isFloat() const
    {
      return is_float_;
    }

    bool isBool() const
    {
      return is_bool_;
    }

  protected:
    bool is_int_ = false;
    int32_t int_val_;
    bool is_float_ = false;
    float float_val_;
    bool is_bool_ = false;
    bool bool_val_;
  };

  class Allocate : public User
  {
  public:
    Allocate(std::shared_ptr<Type> type, std::string name) : User(MakePointerType(type->copy()), std::move(name)), type_(std::move(type)) { is_allocate_ = true; };
    std::string dump(DumpHelper &helper) const override
    {
      std::string output = (is_const_ ? "Constant " : "Allocate ") + getName() + ": " + getType()->dump();
      helper.add(output);
      return output;
    }

    void setConst(bool is_const)
    {
      is_const_ = is_const;
    }
    bool getConst() const
    {
      return is_const_;
    }

  protected:
    std::shared_ptr<Type> type_;
    bool is_const_ = false;
  };

  class Load : public Instruction
  {
  public:
    Load(std::shared_ptr<Value> ptr, std::string name) : Instruction(ptr->getType(), std::move(name), 1), ptr_(ptr, this){};
    std::string dump(DumpHelper &helper) const override
    {
      std::string output = "Load " + ptr_.getValue()->dump(helper);
      helper.add(output);
      return output;
    }

  protected:
    Use ptr_;
  };

  class Store : public Instruction
  {
  public:
    Store(std::shared_ptr<Value> val, std::shared_ptr<Value> ptr) : Instruction(MakePrimitiveDataType(PrimitiveDataType::TypeID::Void), "Store", 2), val_(val, this), ptr_(ptr, this){};

    std::string dump(DumpHelper &helper) const override
    {
      std::string output = "Store " + val_.getValue()->dump(helper) + " " + ptr_.getValue()->dump(helper);
      helper.add(output);
      return output;
    }

  protected:
    Use val_;
    Use ptr_;
  };

  class GetElementPtr : public Instruction
  {
  };

  class ExtractValue : public Instruction
  {
  };

  class InsertValue : public Instruction
  {
  };

  class VirtualRegister : public User
  {
  public:
    VirtualRegister(std::shared_ptr<Type> type, std::shared_ptr<User> val, size_t count, std::string name) : User(type, std::move(name)), val_(val, this), count_(count) { is_virtual_register_ = true; };
    std::string dump(DumpHelper &helper) const override
    {
      std::string output = "VirtualRegister " + getName() + ": " + getType()->dump();
      helper.add(output);
      return output;
    }

  protected:
    Use val_;
    size_t count_ = 0;
  };

}
