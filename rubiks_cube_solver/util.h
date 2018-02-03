#pragma once

#include <random>
#include <functional>
#include <limits>
#include <initializer_list>
#include <iterator>
#include <iostream>

using namespace std;

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


template<typename T, typename Predicate>
bool exists(vector<T> source, Predicate predicate) {
	auto res =  any_of(source.begin(), source.end(), predicate );
	return res;
}

template<typename T, typename Predicate>
T findin(vector<T> source, Predicate predicate) {
	return *find_if(source.begin(), source.end(), predicate);
}

template<typename T, typename Predicate>
bool forall(vector<T> c, Predicate predicate) {
	return all_of(c.begin(), c.end(), predicate);
}

template<typename A, typename B, typename Func>
vector<B> mapf(vector<A> source, Func f) {
	vector<B> res(source.size());
	transform(source.begin(), source.end(), res.begin(), f);
	return res;
}

template<typename T, typename Consume>
void foreach(vector<T> source, Consume consume) {
	for_each(source.begin(), source.end(), consume );
}