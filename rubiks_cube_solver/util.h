#pragma once

#include <random>
#include <functional>
#include <limits>
#include <initializer_list>
#include <iterator>
#include <iostream>

auto rng = []() {
	std::random_device rnd;
	auto eng = std::default_random_engine{ rnd() };
	auto dist = std::uniform_real_distribution<float>{ 0.0, 1.0 };
	return std::bind(dist, eng);
}();

unsigned int nextInt(unsigned x = std::numeric_limits<unsigned>::max()) {
	return floor(rng() * x);
}

template<typename T, typename Predicate>
vector<T> filter(vector<T> source, Predicate predicate) {
	auto end = remove_if(source.begin(), source.end(), [&](T t) { return !predicate(t); });
	return vector<T>{source.begin(), end};
}