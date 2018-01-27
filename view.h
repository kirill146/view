#pragma once

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
		filtered_container const* fc;
		typename Container::const_iterator it;

		iterator(filtered_container const* fc, typename Container::const_iterator it)
			: fc(fc)
			, it(it)
		{}

	public:
		iterator() = default;
		iterator(iterator const&) = default;
		iterator& operator=(iterator const&) = default;

		iterator& operator++() {
			++it;
			while (it != fc->c.end() && !fc->f(*it)) {
				++it;
			}
			return *this;
		}

		iterator& operator--() {
			--it;
			while (!fc->f(*it)) {
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
		return iterator(this, it);
	}

	iterator end() const {
		return iterator(this, c.end());
	}

private:
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
		transformed_container const* tc;
		typename Container::const_iterator it;

		iterator(transformed_container const* tc, typename Container::const_iterator it)
			: tc(tc)
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
			return tc->f(*it);
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
		return iterator(this, c.begin());
	}

	iterator end() const {
		return iterator(this, c.end());
	}

private:
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

private:
	Container* c;
};

template <typename Container>
ref_container<Container> ref(Container& c) {
	return ref_container<Container>(c);
}