#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
using namespace std;

void init(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process);
bool best(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process);
bool first(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process);
void dealloc(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process, unsigned short num, bool pre);

int main(){
	map<unsigned long, string> memory;				// memory segments and info
	map<unsigned long, unsigned long> free;			// base addresses and sizes of free holes
	vector< map<string, unsigned long> > process;	// process segments info
	// memory<base address, segment info>
	// free<base address, hole size>
	// process[1] = map<segment name, base address>
	// process[0] is reserved for initially allocated segments

	while(1){
		char choice;
		cin >> choice;

		// initialize
		if (choice == 'i' || choice == 'I')
			init(memory, free, process);

		// best fit
		else if (choice == 'b' || choice == 'B'){
			if (best(memory, free, process))
				cout << "\nSuccessfully Allocated <3\n";
			else
				cout << "\nFailed to allocate :(\n";
		}

		// first fit
		else if (choice == 'f' || choice == 'F'){
			if (first(memory, free, process))
				cout << "\nSuccessfully Allocated <3\n";
			else
				cout << "\nFailed to allocate :(\n";
		}

		// deallocate
		else if (choice == 'd' || choice == 'D'){
			char type;
			unsigned short num;
			cin >> type >> num;
			if (type == 'i' || type == 'I')
				dealloc(memory, free, process, num, 1);
			else
				dealloc(memory, free, process, num, 0);
		}

		else if (choice == 'q' || choice == 'Q')
			break;

		// memory print
		for (auto it = memory.begin(); it != memory.end(); ++it)
			cout << it->first << " [" << it->second << "] ";
		cout << endl << endl;

		// segment tables print
		for (size_t i = 0; i < process.size(); ++i){
			if (!process[i].empty()){
				cout << "\t   Process " << i << endl;
				cout << "num\tname\tbase\tsize\n";
				unsigned short j = 0;
				for (auto it = process[i].begin(); it != process[i].end(); ++it){
					cout << j << "\t" << it->first << "\t" << it->second << "\t";
					auto next = memory.find(it->second);
					++next;
					cout << next->first - it->second << endl;
					++j;
				}
				cout << endl;
			}
		}
	}
	return 0;
}

void init(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process){
	unsigned long MemorySZ;							// total memory size
	unsigned short N;								// number of initial holes
	cin >> MemorySZ >> N;

	vector<unsigned long> base(N);					// base addresses of initial holes
	for (unsigned short i = 0; i < N; ++i)
		cin >> base[i];

	vector<unsigned long> size(N);					// sizes of initial holes
	for (unsigned short i = 0; i < N; ++i)
		cin >> size[i];

	process.push_back(map<string, unsigned long>());

	// add holes
	for (unsigned short i = 0; i < N; ++i){
		free.insert({base[i],size[i]});
		memory.insert({base[i],""});
	}

	// if there is two adjacent holes
	auto it = free.begin();
	auto next = it;
	++next;
	while (next != free.end()){
		if (it->first + it->second == next->first){
			it->second += next->second;
			memory.erase(next->first);
			free.erase(next);
			next = it;
		}
		else
			++it;
		++next;
	}

	// if there is a free hole at address 0
	if (free.begin()->first == 0)
		memory.insert({0,""});
	else {
		memory.insert({0,"Preallocated 0"});
		process[0]["0"] = 0;
	}
	memory.insert({MemorySZ,""});

	// determine preallocated segments
	for (it = free.begin(); it != free.end(); ++it){
		memory[it->first + it->second] = "Preallocated " + to_string(process[0].size());
		process[0][to_string(process[0].size())] = it->first + it->second;
	}

	// correct the last hole
	if (!memory[MemorySZ].empty()){
		memory[MemorySZ] = "";
		auto last = process[0].end();
		--last;
		process[0].erase(last);
	}
}

bool best(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process){
	unsigned short N;								// number of segments
	cin >> N;

	vector<string> name(N);							// segments names
	for (unsigned short i = 0; i < N; ++i)
		cin >> name[i];

	vector<unsigned long> size(N);					// segments sizes
	for (unsigned short i = 0; i < N; ++i)
		cin >> size[i];

	map<unsigned long, unsigned long> FREE(free.begin(), free.end());
	vector<unsigned long> base;

	// best fit
	for (unsigned short i = 0; i < N; ++i){
		// determine first fit
		unsigned long min;
		auto it = FREE.begin();
		while (it != FREE.end()){
			if (size[i] <= it->second){
				min = it->first;
				break;
			}
			++it;
		}

		// check if no block fits the segement
		if (it == FREE.end())
			return false;

		// determine best fit
		++it;
		while (it != FREE.end()){
			if (size[i] <= it->second && it->second < FREE[min])
				min = it->first;
			++it;
		}

		// update FREE
		base.push_back(min);
		if (size[i] < FREE[min])
			FREE[min + size[i]] = FREE[min] - size[i];
		FREE.erase(min);
	}

	// update free
	free = FREE;

	// update memory
	for (unsigned short i = 0; i < N; ++i){
		memory[base[i]] = "P" + to_string(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
	process.push_back(map<string, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

bool first(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process){
	unsigned short N;								// number of segments
	cin >> N;

	vector<string> name(N);							// segments names
	for (unsigned short i = 0; i < N; ++i)
		cin >> name[i];

	vector<unsigned long> size(N);					// segments sizes
	for (unsigned short i = 0; i < N; ++i)
		cin >> size[i];

	map<unsigned long, unsigned long> FREE(free.begin(), free.end());
	vector<unsigned long> base;

	// first fit
	for (unsigned short i = 0; i < N; ++i){
		for (auto it = FREE.begin(); it != FREE.end(); ++it){
			if (size[i] <= it->second){
				base.push_back(it->first);
				if (size[i] < it->second)
					FREE[it->first + size[i]] = it->second - size[i];
				FREE.erase(it);
				break;
			}
		}
		if (base.size() == i)
			return false;
	}

	// update free
	free = FREE;

	// update memory
	for (unsigned short i = 0; i < N; ++i){
		memory[base[i]] = "P" + to_string(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
	process.push_back(map<string, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

void dealloc(map<unsigned long, string> &memory, map<unsigned long, unsigned long> &free, vector< map<string, unsigned long> > &process, unsigned short num, bool pre){
	// deallocate a preallocated block
	if (pre){
		map<string,unsigned long>::iterator p = process[0].find(to_string(num));
		auto last = memory.end();				// iterator to last memory block
		--last;
		--last;

		// segment is allocated at address 0
		if(p->second == 0){
			auto next = memory.begin();
			++next;

			// if next segment is free
			if((next->second).empty()){
				free[0] = next->first + free[next->first];
				free.erase(next->first);
				memory.erase(next);
			}
			// if next segment is allocated
			else
				free[0] = next->first;

			memory[0] = "";
		}

		// segment is allocated at last memory block
		else if (p->second == last->first){
			auto prev = last;
			--prev;

			// if previous segment is free
			if ((prev->second).empty()){
				free[prev->first] += memory.rbegin()->first - last->first;
				memory.erase(last);
			}
			// if previous segment is allocated
			else {
				free[last->first] = memory.rbegin()->first - last->first;
				last->second = "";
			}
		}

		// segment is allocated in an inner block
		else {
			auto prev = memory.find(p->second);
			auto next = memory.find(p->second);
			--prev;
			++next;

			// if both prev and next are free
			if ((prev->second).empty() && (next->second).empty()){
				free[prev->first] += next->first - p->second + free[next->first];
				free.erase(next->first);
				memory.erase(p->second);
				memory.erase(next);
			}

			// if only next is free
			else if ((next->second).empty()){
				free[p->second] = next->first - p->second + free[next->first];
				free.erase(next->first);
				memory.erase(next);
				memory[p->second] = "";
			}

			// if only prev is free
			else if ((prev->second).empty()){
				free[prev->first] += next->first - p->second;
				memory.erase(p->second);
			}

			// if both prev and next are allocated
			else {
				free[p->second] = next->first - p->second;
				memory[p->second] = "";
			}
		}
		process[0].erase(p);
	}

	// deallocate a user allocated block
	else {
		for (auto it = process[num].begin(); it != process[num].end(); ++it){
			auto last = memory.end();				// iterator to last memory block
			--last;
			--last;

			// segment is allocated at address 0
			if (it->second == 0){
				auto next = memory.begin();
				++next;

				// if next segment is free
				if ((next->second).empty()){
					free[0] = next->first + free[next->first];
					free.erase(next->first);
					memory.erase(next);
				}
				// if next segment is allocated
				else
					free[0] = next->first;

				memory[0] = "";
			}

			// segment is allocated at last memory block
			else if (it->second == last->first){
				auto prev = last;
				--prev;

				// if previous segment is free
				if ((prev->second).empty()){
					free[prev->first] += memory.rbegin()->first - last->first;
					memory.erase(last);
				}
				// if previous segment is allocated
				else {
					free[last->first] = memory.rbegin()->first - last->first;
					last->second = "";
				}
			}

			// segment is allocated in an inner block
			else {
				auto prev = memory.find(it->second);
				auto next = memory.find(it->second);
				--prev;
				++next;

				// if both prev and next are free
				if ((prev->second).empty() && (next->second).empty()){
					free[prev->first] += next->first - it->second + free[next->first];
					free.erase(next->first);
					memory.erase(it->second);
					memory.erase(next);
				}

				// if only next is free
				else if ((next->second).empty()){
					free[it->second] = next->first - it->second + free[next->first];
					free.erase(next->first);
					memory.erase(next);
					memory[it->second] = "";
				}

				// if only prev is free
				else if ((prev->second).empty()){
					free[prev->first] += next->first - it->second;
					memory.erase(it->second);
				}

				// if both prev and next are allocated
				else {
					free[it->second] = next->first - it->second;
					memory[it->second] = "";
				}
			}
		}
		process[num].clear();
	}
}


