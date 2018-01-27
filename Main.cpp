#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "view.h"

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

void copy_iterator_test() {
	filtered_container<std::vector<int>, fo>::iterator it1, it2;
	std::vector<int> v{ 1, 2, 3 };
	auto c = filtered(v, fo());
	it1 = c.begin();
	it2 = it1;
	filtered_container<std::vector<int>, fo>::iterator it3(it2);

	transformed_container<std::vector<int>, fo>::iterator jt1, jt2;
	jt1 = jt2;
}

int main() {
	filtered_test();
	transformed_test();
	ref_test();
	other_test();
	other_test2();
	copy_iterator_test();
}