
#include "pair_iterator.h"
#include <iostream>
#include <vector>

using namespace sequoia;

int main(int argc, char **argv) {
	typedef int t1;
	typedef std::string t2;
	std::vector<t1> a;
	for (int i = 0; i < 10 ; i++) a.push_back(i);
	std::vector<t2> b;
	b.push_back("A");
	b.push_back("B");
	b.push_back("C");

	typedef typename std::vector<t1>::iterator it1_it;
	typedef typename std::vector<t2>::iterator it2_it;

	it1_it it1 = a.begin(), it1end = a.end();
	it2_it it2 = b.begin(), it2end = b.end();

	typedef PairIterator<it1_it, it2_it> pair_it;
	pair_it it(std::make_pair(it1, it1end), std::make_pair(it2, it2end));

	while (it.has_next()) {
		typename pair_it::value_type res = it.next();
		std::cout << res.first << "/" << res.second << std::endl;
	}



}


