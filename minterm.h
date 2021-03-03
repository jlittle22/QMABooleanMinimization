#ifndef MINTERM_H
#define MINTERM_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

class Minterm {
  public:
  	// constructor from single minterm
    Minterm(int mv, bool dc, string ab);

    // constructor from multiple minterms
    Minterm(const Minterm& m1, const Minterm& m2, string ab);

    // destructor 
    ~Minterm();

    // gets a vector copy of a Minterm's represented values
  	vector<int> getMintermValues() const;

  	// checks if a minterm contains a certain value
  	bool contains(int elem) const;

  	// checks if it's an X minterm
  	bool isDontCare() const;

  	// return the minterm's binary representation
  	string getAsBinary() const;

  	// return the minterm's logic representation
  	string getAsLogic() const;

  	// sets the minterm as matched or unmatched
  	void setMatched(bool b);

  	// checks if minterm is matched
  	bool isMatched();

    // equality operator
  	bool operator==(const Minterm& other) const;

    // outstream operator
  	friend ostream& operator<<(ostream& os, const Minterm& mt);
 
    // Minterm hash function
  	struct hash_fn {
        size_t operator() (const Minterm &m) const {
        	size_t h1 = 0;
        	for(auto& i : m.mintermValues) {
                h1 = (h1 << (size_t)ceil(log2(i))) + i;
        	}
            return h1;
        }
    };

  private:
  	// vector of minterm values
  	vector<int> mintermValues;

  	// binary rep.
    string asBinary;

    // duh
    bool dontCare;

    // duh
    bool matched;

};

#endif