
struct Slope {
  int n, d;
  Slope (int nn, int dd) {
    int g = gcd(abs(nn), abs(dd));
    // cout << "gcd of " << nn << ", " << dd << " is: " << gg << endl;
    n = (g>0 ? nn/g : nn);
    d = (g>0 ? dd/g : dd);
  }
};

struct Cmp {
  size_t operator()(const Slope& k) const {
    return (k.n<<3)^k.d;
  }

  size_t operator()(const Slope& k1, const Slope& k2) const {
    return k1.n == k2.n && k1.d == k2.d;
  }
};

vector<unordered_map<Slope, vector<int>, Cmp, Cmp>> v;

