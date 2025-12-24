/**
 * 
 * builder 建造者模式，生成器模式
 * 
 *  可避免重叠构造函数，如果有十个可选参数，那么调用该函数会非常不便；
 *   
 */

#include <iostream>
#include <vector>
#include <string>


class Product1
{
public:
    std::vector<std::string> parts_;
    void LisParts() const 
    {
        std::cout << "Product parts: ";
        for (size_t i = 0; i < parts_.size(); i++)
        {
            if (parts_[i] == parts_.back())
            {
                std::cout << "parts_[" << i << "]:" << parts_[i];
            } else {
                std::cout << "parts_[" << i << "]:" << parts_[i] << ", ";
            }
        }
        std::cout << "\n\n";
    }
};

/**
 * 生成器接口用来创建不同产品对象的方法
 */
class Builder {
public:
    virtual ~Builder(){}
    virtual void ProducePartA() const = 0;
    virtual void ProducePartB() const = 0;
    virtual void ProducePartC() const = 0;
};

/**
 * 
 * 特定生成器类遵循生成器接口，提供特定生成器类型的实现。程序会有多个生成器的变体，实现也各不相同
 */
class ConcreteBuilder1 : public Builder{
private:
    Product1 *product;
public:
    ConcreteBuilder1() {
        this->Reset();
    }

    ~ConcreteBuilder1() {
        delete product;
    }

    void Reset() {
        this->product = new Product1();
    }

    void ProducePartA() const override {
        this->product->parts_.push_back("PartA1");
    }

    void ProducePartB() const override {
        this->product->parts_.push_back("PartB1");
    }

    void ProducePartC() const override {
        this->product->parts_.push_back("PartC1");
    }

    Product1 *GetProduct() {
        Product1 *result = this->product;
        this->Reset();
        return result;
    }
};

/**
 * 主管类用来负责在执行生成的特定步骤序列。对于根据特定顺序或配置生产产品时是很有用的。
 * 
 * 严格来讲，主管类是可选的，因为客户端是可以直接控制生成器的。
 */

class Director{
private:
    Builder *builder_;

public:
    void set_builder(Builder *builder) {
        this->builder_ = builder;
    }

    void BuildMinimalViableProduct() {
        this->builder_->ProducePartA();
    }

    void BuildFullFeaturProduct() {
        this->builder_->ProducePartA();
        this->builder_->ProducePartB();
        this->builder_->ProducePartC();
    }
};

void ClientCode(Director& director)
{
    ConcreteBuilder1 *builder = new ConcreteBuilder1();
    director.set_builder(builder);
    std::cout << "标准基本产品：" << std::endl;
    director.BuildMinimalViableProduct();

    Product1 *p =builder->GetProduct();
    p->LisParts();
    delete p;

    std::cout << "标准全功能产品：" << std::endl;
    director.BuildFullFeaturProduct();

    p = builder->GetProduct();
    p->LisParts();
    delete p;

    // 生成器模式可以不用主管类Director，直接操作生成器
    std::cout << "自定义产品：" << std::endl;
    builder->ProducePartA();
    builder->ProducePartC();
    p = builder->GetProduct();
    p->LisParts();
    delete p;

    delete builder;
}

int main()
{
    Director *director = new Director();
    ClientCode(*director);
    delete director;
    return 0;
}
