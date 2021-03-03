#include "minterm.h"

// utility function to combine two vectors
static vector<int> joinVectors(const vector<int>& v1, const vector<int>& v2) {
	vector<int> v(v1);
    v.insert(v.end(), v2.begin(), v2.end());
    return v;
}

// ... yep
Minterm::~Minterm() { };

// create minterm from one minterm value
Minterm::Minterm(int mv, bool dc, string ab) {
	mintermValues.push_back(mv);
    dontCare = dc;
    asBinary = ab;
    matched = false;
}

// create minterm from two minterms
Minterm::Minterm(const Minterm& m1, const Minterm& m2, string ab) {
	mintermValues = joinVectors(m1.getMintermValues(), m2.getMintermValues());
    asBinary = ab;
    dontCare = false;
    matched = false;
}

// sets matched
void Minterm::setMatched(bool b) {
	matched = b;
}

// duh
bool Minterm::isMatched() {
	return matched;
}

// utility function to compare two vectors for set equality
static bool compareVectors(vector<int> v, vector<int> x) {
	sort(v.begin(), v.end());
	sort(x.begin(), x.end());
    return v == x;
}

// equality op to compare minterms
bool Minterm::operator==(const Minterm& other) const {
	bool vecs = compareVectors(mintermValues, other.mintermValues);
	bool bin = asBinary == other.asBinary;
    bool dc = dontCare == other.dontCare;

    return (vecs && bin && dc);
}

// print minterms (as logic)!
ostream& operator<<(ostream& os, const Minterm& mt) {
	os << mt.getAsLogic() << " [ ";
	for (auto& v : mt.mintermValues) {
		os << v << " ";
	}
	os << "]";
	return os;
}

// get the mintermValues vector
vector<int> Minterm::getMintermValues() const {
	return mintermValues;
}
 
// duh
bool Minterm::isDontCare() const {
	return dontCare;
}

// duh
string Minterm::getAsBinary() const {
	return asBinary;
}

// converts a minterm's binary representation to a logic equation
string Minterm::getAsLogic() const {
	string logic = "";
	// for each character in binary ...
	for (unsigned i = 0; i < asBinary.length(); i++) {
		// add 65 to convert variable to ascii
		char c = i + 65;
		// if binary is a 1, just add variable
		if (asBinary[i] == '1') {
			logic += c;
		// if it's a 0, add the not operator
		} else if (asBinary[i] == '0') {
			logic += '!';
			logic += c;
		}
		// if it's a '-', do nothing
	}
	return logic;
}

// returns check elem exists in mintermValues
bool Minterm::contains(int elem) const {
    for (auto& i : mintermValues) {
    	if (i == elem) return true;
    }
	return false;
}
