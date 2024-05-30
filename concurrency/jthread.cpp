// c++20 jthread to handle join in the destruct function
//
#include <iostream>
#include <thread>

//int main(){
/*	std::thread t([] {});
	try{
	  throw 0;
	} catch (int x) {
	  t.join();
	  throw x;
	}
	t.join();*/
//	std::jthread t([] {});
//}
class Jthread {
	 public:
		   Jthread() noexcept = default;

		     template <typename T, typename... Ts>
			       explicit Jthread(T&& f, Ts&&... args)
			             : t_(std::forward<T>(f), std::forward<Ts>(args)...) {}

		       explicit Jthread(std::thread x) noexcept : t_(std::move(x)) {}
		         Jthread(Jthread&& rhs) noexcept : t_(std::move(rhs.t_)) {}

			   Jthread& operator=(Jthread&& rhs) noexcept {
				       if (joinable()) {
					             join();
						         }
				           t_ = std::move(rhs.t_);
					       return *this;
					         }

			     Jthread& operator=(std::thread t) noexcept {
				         if (joinable()) {
						       join();
						           }
					     t_ = std::move(t);
					         return *this;
						   }

			       ~Jthread() noexcept {
				           if (joinable()) {
						         join();
							     }
					     }

			         void swap(Jthread&& rhs) noexcept { t_.swap(rhs.t_); }
				   std::thread::id get_id() const noexcept { return t_.get_id(); }
				     bool joinable() const noexcept { return t_.joinable(); }
				       void join() { t_.join(); }
				         void detach() { t_.detach(); }
					   std::thread& as_thread() noexcept { return t_; }
					     const std::thread& as_thread() const noexcept { return t_; }

					      private:
					       std::thread t_;
};

int main() {
	  Jthread t{[] {}};
}
