#include <iostream>
#include <vector>
#include <string>
#include <sstream>

template <typename F>
struct func_type {
	typedef std::remove_reference_t<F> type;
};

template <typename T, typename ... Args>
struct func_type<T(&)(Args...)> {
	typedef T(*type)(Args...);
};

template <typename F>
using func_type_t = typename func_type<F>::type;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Container, typename Predicate>
struct filtered_container {
	template <typename C, typename F>
	filtered_container(C&& c, F&& f)
		: c(std::forward<C>(c))
		, f(std::forward<F>(f))
	{}

	typedef typename Container::value_type value_type;

	struct iterator {
		friend filtered_container;
		
	private:
		Container const* c;
		Predicate const& f;
		typename Container::const_iterator it;

		iterator(Container const* c, Predicate const& f, typename Container::const_iterator it)
			: c(c)
			, f(f)
			, it(it)
		{}

	public:
		iterator() = default;
		iterator(iterator const&) = default;
		iterator& operator=(iterator const&) = default;

		iterator& operator++() {
			++it;
			while (it != c->end() && !f(*it)) {
				++it;
			}
			return *this;
		}

		iterator& operator--() {
			--it;
			while (!f(*it)) {
				--it;
			}
			return *this;
		}

		iterator operator++(int) {
			iterator res(*this);
			++*this;
			return res;
		}

		iterator operator--(int) {
			iterator res(*this);
			--*this;
			return res;
		}

		value_type const& operator*() const {
			return *it;
		}

		friend bool operator==(typename filtered_container<Container, Predicate>::iterator const& a,
							   typename filtered_container<Container, Predicate>::iterator const& b) {
			return a.it == b.it;
		}

		friend bool operator!=(typename filtered_container<Container, Predicate>::iterator const& a,
							   typename filtered_container<Container, Predicate>::iterator const& b) {
			return a.it != b.it;
		}
	};
	typedef iterator const_iterator;

	iterator begin() const {
		auto it = c.begin();
		while (it != c.end() && !f(*it)) {
			++it;
		}
		return iterator(&c, f, it);
	}

	iterator end() const {
		return iterator(&c, f, c.end());
	}

	Container c;
	Predicate f;
};

template <typename Container, typename Predicate>
filtered_container<std::remove_reference_t<Container>, func_type_t<Predicate>> filtered(Container&& c, Predicate&& f) {
	return filtered_container<std::remove_reference_t<Container>, func_type_t<Predicate>>(std::forward<Container>(c), std::forward<Predicate>(f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Container, typename F>
struct transformed_container {
	template <typename C, typename Func>
	transformed_container(C&& c, Func&& f)
		: c(std::forward<C>(c))
		, f(std::forward<F>(f))
	{}

	typedef std::result_of_t<F(typename Container::value_type)> value_type;

	struct iterator {
		friend transformed_container;

	private:
		Container const* c;
		F const& f;
		typename Container::const_iterator it;

		iterator(Container const* c, F const& f, typename Container::const_iterator it)
			: c(c)
			, f(f)
			, it(it)
		{}

	public:
		iterator() = default;
		iterator(iterator const&) = default;
		iterator& operator=(iterator const&) = default;

		iterator& operator++() {
			++it;
			return *this;
		}

		iterator& operator--() {
			--it;
			return *this;
		}

		iterator operator++(int) {
			iterator res(*this);
			++*this;
			return res;
		}

		iterator operator--(int) {
			iterator res(*this);
			--*this;
			return res;
		}

		value_type operator*() const {
			return f(*it);
		}

		friend bool operator==(typename transformed_container<Container, F>::iterator const& a,
			typename transformed_container<Container, F>::iterator const& b) {
			return a.it == b.it;
		}

		friend bool operator!=(typename transformed_container<Container, F>::iterator const& a,
			typename transformed_container<Container, F>::iterator const& b) {
			return a.it != b.it;
		}
	};
	typedef iterator const_iterator;

	iterator begin() const {
		return iterator(&c, f, c.begin());
	}

	iterator end() const {
		return iterator(&c, f, c.end());
	}

	Container c;
	F f;
};

template <typename Container, typename F>
transformed_container<std::remove_reference_t<Container>, func_type_t<F>> transformed(Container&& c, F&& f) {
	return transformed_container<std::remove_reference_t<Container>, func_type_t<F>>(std::forward<Container>(c), std::forward<F>(f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Container>
struct ref_container {
	ref_container(Container& c)
		: c(&c)
	{}

	typedef typename Container::value_type value_type;

	typedef typename Container::iterator iterator;
	typedef typename Container::iterator const_iterator;

	iterator begin() {
		return iterator(c->begin());
	}

	const_iterator begin() const {
		return const_iterator(c->begin());
	}

	iterator end() {
		return iterator(c->end());
	}

	const_iterator end() const {
		return const_iterator(c->end());
	}

	Container* c;
};

template <typename Container>
ref_container<Container> ref(Container& c) {
	return ref_container<Container>(c);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// tests
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void filtered_test() {
	std::vector<int> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	auto cc = filtered(v, [](int a) -> bool { return a % 2 == 0; });
	auto c = filtered(cc, [](int a) -> bool { return a != 4; });
	for (auto it = c.begin(); it != c.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

void transformed_test() {
	std::vector<int> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	auto cc = transformed(v, [](int a) -> int { return a * 2; });
	auto c = transformed(cc, [](int a) -> std::string {
		std::stringstream ss;
		ss << '"' << a << '"';
		return ss.str();
	});
	for (auto it = c.begin(); it != c.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

void ref_test() {
	std::vector<int> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	auto c = ::ref(v);
	*c.begin() = 301;
	auto cc = filtered(::ref(v), [](int a) { return a % 3 != 0;  });
	for (auto it = cc.begin(); it != cc.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

bool g(int a) {
	return a % 2 == 1;
}

void other_test() {
	std::vector<int> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	auto c = filtered(v, g);
	for (auto it = c.begin(); it != c.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

struct fo {
	bool operator()(int a) const {
		return a % 2 == 1;
	}
};

void other_test2() {
	std::vector<int> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	auto c = filtered(v, fo());
	for (auto it = c.begin(); it != c.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

int main() {
	filtered_test();
	transformed_test();
	ref_test();
	other_test();
	other_test2();
}