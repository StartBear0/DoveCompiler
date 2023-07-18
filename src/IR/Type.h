#pragma once

#include <memory>
#include <string>

namespace ir
{
  class Type
  {
  public:
    virtual ~Type() = default;
    virtual std::shared_ptr<Type> get() const = 0;
    virtual std::shared_ptr<Type> copy() const = 0;
    virtual std::string dump() = 0;

    bool isPrimitive() const
    {
      return is_primitive_;
    }
    bool isPointer() const
    {
      return is_pointer_;
    }
    bool isArray() const
    {
      return is_array_;
    }

  protected:
    bool is_primitive_ = false;
    bool is_pointer_ = false;
    bool is_array_ = false;
  };

  class PrimitiveDataType : public Type
  {
  public:
    enum class TypeID
    {
      Int32,
      Float32,
      Boolean,
      Void,
    };

    PrimitiveDataType(TypeID id) : type_(id)
    {
      is_primitive_ = true;
    }
    std::shared_ptr<Type> get() const
    {
      return std::make_shared<PrimitiveDataType>(type_);
    }
    std::shared_ptr<Type> copy() const
    {
      return std::make_shared<PrimitiveDataType>(type_);
    }
    std::string dump()
    {
      switch (type_)
      {
      case TypeID::Int32:
        return "Int32";
      case TypeID::Float32:
        return "Float32";
      case TypeID::Boolean:
        return "Boolean";
      case TypeID::Void:
        return "Void";
      default:
        return "Unknown";
      }
    }

  protected:
    TypeID type_;
  };
  std::shared_ptr<PrimitiveDataType> MakePrimitiveDataType(PrimitiveDataType::TypeID id);

  class PointerType : public Type
  {
  public:
    explicit PointerType(std::shared_ptr<Type> ref) : ref_(ref)
    {
      is_pointer_ = true;
    };
    PointerType(const PointerType &other) : ref_(other.ref_->get()) {}
    std::shared_ptr<Type> get() const
    {
      return ref_->get();
    }
    std::shared_ptr<Type> copy() const
    {
      return std::make_shared<PointerType>(std::make_shared<PointerType>(ref_->copy()));
    }
    std::string dump()
    {
      return "" + ref_->dump() + "*";
    }

  protected:
    std::shared_ptr<Type> ref_;
  };

  std::shared_ptr<PointerType> MakePointerType(std::shared_ptr<Type> ref);

  


  std::shared_ptr<Type> MakeType(std::shared_ptr<Type> ptr);

 class ArrayType : public Type
  {
  public:
    explicit ArrayType(std::shared_ptr<Type>  eltype,size_t len) :  eltype_( eltype) ,len_(len){}
    explicit ArrayType(const ArrayType &other) :  eltype_(other.eltype_->get()), len_(other.getLen()) {}
    std::shared_ptr<Type> get() const
    {
      return  eltype_->get();
    }
    std::shared_ptr<Type> copy() const
    {
      return std::make_shared<ArrayType>(std::make_shared<ArrayType>( eltype_->copy()),len_);
    }
    std::string dump()
    {
      return "" +  eltype_->dump() + "[" + std::to_string(int(len_)) + "]";
    }
    size_t getLen() const
    {
      return len_;
    }
    void setLen(size_t len) 
    {
      len_=len;
    }

   protected:
    std::shared_ptr<Type>  eltype_;
    size_t len_;
  };

  std::shared_ptr<ArrayType> MakeArrayType(std::shared_ptr<Type>  eltype,size_t len);

}
