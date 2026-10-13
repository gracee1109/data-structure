#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace nccu {
using namespace std;
using U64 = uint64_t;
using Matrix = vector<vector<bool>>;
using WeightMatrix = vector<vector<long long>>;
inline constexpr long long INF = numeric_limits<long long>::max() / 4;

inline const vector<int> keys114{31,82,14,3,12,6,7,95,10,162,214,36,9,2,138,78,22,56};
inline const vector<int> keys113{1,8,4,53,12,7,35,103,16,14,63,29,20,72,38,18,22,56};
inline const vector<int> keys112{108,72,99,29,97,22,69,120,208,27,33,53,54,48,26,49};
inline const vector<int> keys111{17,35,12,28,16,5,18,7,19,92,48,3,22,1};

template<class T> void printVector(const vector<T>& a, ostream& out = cout) {
    for (size_t i = 0; i < a.size(); ++i) {
        if (i) out << ' ';
        out << a[i];
    }
    out << '\n';
}

template<class T> struct Node {
    T key;
    int height = 1;
    unique_ptr<Node<T>> left, right;
    explicit Node(T value): key(move(value)) {}
};
template<class T> using Ptr = unique_ptr<Node<T>>;
template<class T> int height(const Ptr<T>& p) { return p ? p->height : 0; }
template<class T> void pull(Node<T>* p) {
    if (p) p->height = 1 + max(height(p->left), height(p->right));
}
template<class T> Ptr<T> node(T value, Ptr<T> left = {}, Ptr<T> right = {}) {
    auto p = make_unique<Node<T>>(move(value));
    p->left = move(left); p->right = move(right); pull(p.get());
    return p;
}
template<class T> void traverse(const Node<T>* p, int order, vector<T>& output) {
    if (!p) return;
    if (order == 0) output.push_back(p->key);
    traverse(p->left.get(), order, output);
    if (order == 1) output.push_back(p->key);
    traverse(p->right.get(), order, output);
    if (order == 2) output.push_back(p->key);
}
template<class T> vector<T> traversal(const Ptr<T>& p, int order) {
    vector<T> out; traverse(p.get(), order, out); return out;
}
// 無損的樹形表示法 key(left,right)，空子樹為「.」。
template<class T> void printTree(const Node<T>* p, ostream& out = cout) {
    if (!p) { out << '.'; return; }
    out << p->key << '('; printTree(p->left.get(), out); out << ',';
    printTree(p->right.get(), out); out << ')';
}
inline Ptr<string> companyTree() {
    return node<string>("Computers\"R\"Us",
        node<string>("Sales", node<string>("US"),
            node<string>("International", node<string>("Asia"), node<string>("Canada"))),
        node<string>("Manufacturing", node<string>("Laptops"), node<string>("Desktops")));
}
inline const vector<string> companyPre{"Computers\"R\"Us","Sales","US","International","Asia","Canada","Manufacturing","Laptops","Desktops"};
inline const vector<string> companyIn{"US","Sales","Asia","International","Canada","Computers\"R\"Us","Laptops","Manufacturing","Desktops"};
inline const vector<string> companyPost{"US","Asia","Canada","International","Sales","Laptops","Desktops","Manufacturing","Computers\"R\"Us"};
inline int ord(char c) {
    if (c == '"') return 0;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 1;
    if (c >= 'a' && c <= 'z') return c - 'a' + 27;
    throw invalid_argument("Ord accepts ASCII letters and normalized quotes");
}
struct StringLess {
    bool operator()(const string& a, const string& b) const {
        if (a.size() != b.size()) return a.size() < b.size();
        for (size_t i = 0; i < a.size(); ++i) {
            if (ord(a[i]) != ord(b[i])) return ord(a[i]) < ord(b[i]);
        }
        return false;
    }
};

template<class T> void rotateLeft(Ptr<T>& p) {
    auto q = move(p->right); p->right = move(q->left); pull(p.get());
    q->left = move(p); pull(q.get()); p = move(q);
}
template<class T> void rotateRight(Ptr<T>& p) {
    auto q = move(p->left); p->left = move(q->right); pull(p.get());
    q->right = move(p); pull(q.get()); p = move(q);
}
template<class T> void balance(Ptr<T>& p, ostream* log = nullptr) {
    if (!p) return;
    pull(p.get());
    int b = height(p->left) - height(p->right);
    if (b > 1) {
        bool doubleRotation = height(p->left->left) < height(p->left->right);
        if (log) *log << (doubleRotation ? "LR at " : "LL at ") << p->key << '\n';
        if (doubleRotation) rotateLeft(p->left);
        rotateRight(p);
    } else if (b < -1) {
        bool doubleRotation = height(p->right->right) < height(p->right->left);
        if (log) *log << (doubleRotation ? "RL at " : "RR at ") << p->key << '\n';
        if (doubleRotation) rotateRight(p->right);
        rotateLeft(p);
    }
}
template<class T, class Less = less<T>> void avlInsert(Ptr<T>& p, const T& key,
                                                       Less lessThan = {}, ostream* log = nullptr) {
    if (!p) { p = node<T>(key); return; }
    if (lessThan(key, p->key)) avlInsert(p->left, key, lessThan, log);
    else if (lessThan(p->key, key)) avlInsert(p->right, key, lessThan, log);
    else return; // 本批原題鍵值互異；此實作的語意是 set。
    balance(p, log);
}
template<class T, class Less = less<T>> void avlErase(Ptr<T>& p, const T& key,
                                                      Less lessThan = {}, ostream* log = nullptr) {
    if (!p) return;
    if (lessThan(key, p->key)) avlErase(p->left, key, lessThan, log);
    else if (lessThan(p->key, key)) avlErase(p->right, key, lessThan, log);
    else if (!p->left) { auto replacement = move(p->right); p = move(replacement); }
    else if (!p->right) { auto replacement = move(p->left); p = move(replacement); }
    else {
        auto* successor = p->right.get();
        while (successor->left) successor = successor->left.get();
        p->key = successor->key;
        avlErase(p->right, p->key, lessThan, log);
    }
    balance(p, log);
}
template<class T, class Less = less<T>> int checkAVL(const Node<T>* p, Less lessThan = {}) {
    if (!p) return 0;
    auto l = checkAVL(p->left.get(), lessThan), r = checkAVL(p->right.get(), lessThan);
    assert(abs(l - r) <= 1 && p->height == 1 + max(l, r));
    if (p->left) assert(lessThan(p->left->key, p->key));
    if (p->right) assert(lessThan(p->key, p->right->key));
    return p->height;
}
inline void showAVL(const vector<int>& keys, const vector<int>& removeKeys = {}) {
    Ptr<int> root;
    for (int x: keys) {
        avlInsert(root, x, less<int>{}, &cout);
        cout << "insert " << x << ": "; printTree(root.get()); cout << '\n';
        checkAVL(root.get());
    }
    auto expected = keys; sort(expected.begin(), expected.end());
    assert(traversal(root, 1) == expected);
    cout << "sorted: "; printVector(traversal(root, 1));
    for (int x: removeKeys) {
        avlErase(root, x, less<int>{}, &cout);
        expected.erase(lower_bound(expected.begin(), expected.end(), x));
        assert(traversal(root, 1) == expected); checkAVL(root.get());
        cout << "erase " << x << ": "; printTree(root.get()); cout << '\n';
    }
}
inline Ptr<int> splay(Ptr<int> root, int key) {
    if (!root || root->key == key) return root;
    if (key < root->key) {
        if (!root->left) return root;
        if (key < root->left->key) {
            root->left->left = splay(move(root->left->left), key);
            rotateRight(root);
        } else if (key > root->left->key) {
            root->left->right = splay(move(root->left->right), key);
            if (root->left->right) rotateLeft(root->left);
        }
        if (root->left) rotateRight(root);
    } else {
        if (!root->right) return root;
        if (key > root->right->key) {
            root->right->right = splay(move(root->right->right), key);
            rotateLeft(root);
        } else if (key < root->right->key) {
            root->right->left = splay(move(root->right->left), key);
            if (root->right->left) rotateRight(root->right);
        }
        if (root->right) rotateLeft(root);
    }
    pull(root.get()); return root;
}
inline void bstInsert(Ptr<int>& p, int x) {
    if (!p) { p = node<int>(x); return; }
    if (x < p->key) bstInsert(p->left, x);
    else if (x > p->key) bstInsert(p->right, x);
    pull(p.get());
}

template<class T, class Higher> void heapDown(vector<T>& a, int i, int n, Higher higher) {
    while (2*i + 1 < n) {
        int child = 2*i + 1;
        if (child + 1 < n && higher(a[child+1], a[child])) ++child;
        if (!higher(a[child], a[i])) break;
        swap(a[i], a[child]); i = child;
    }
}
template<class T, class Higher> void heapBuild(vector<T>& a, Higher higher, ostream* log = nullptr) {
    for (int i = int(a.size())/2 - 1; i >= 0; --i) {
        heapDown(a, i, a.size(), higher);
        if (log) { *log << "sift " << i << ": "; printVector(a, *log); }
    }
}
template<class T, class Higher> void heapPush(vector<T>& a, T x, Higher higher) {
    a.push_back(move(x));
    int i = a.size() - 1;
    while (i > 0 && higher(a[i], a[(i-1)/2])) {
        swap(a[i], a[(i-1)/2]); i = (i-1)/2;
    }
}
inline void heapSort(vector<int>& a, ostream* log = nullptr) {
    heapBuild(a, greater<int>{}, log);
    for (int n = a.size(); n > 1; --n) {
        swap(a[0], a[n-1]); heapDown(a, 0, n-1, greater<int>{});
        if (log) { *log << "active=" << n-1 << ": "; printVector(a, *log); }
    }
}
inline vector<int> minHeapSort(vector<int> a, ostream* log = nullptr) {
    heapBuild(a, less<int>{}, log); vector<int> out;
    while (!a.empty()) {
        out.push_back(a[0]); a[0] = a.back(); a.pop_back();
        heapDown(a, 0, a.size(), less<int>{});
        if (log) {
            *log << "extract " << out.back() << ", heap:";
            if (a.empty()) *log << " .\n";
            else { *log << ' '; printVector(a, *log); }
        }
    }
    return out;
}
inline void mergeSort(vector<int>& a, ostream* log = nullptr) {
    vector<int> buffer(a.size());
    function<void(int,int)> go = [&](int l, int r) {
        if (r-l <= 1) return;
        int m = l + (r-l)/2; go(l, m); go(m, r);
        int i=l, j=m, k=l;
        while (i<m || j<r) {
            if (j==r || (i<m && a[i]<=a[j])) buffer[k++] = a[i++];
            else buffer[k++] = a[j++];
        }
        copy(buffer.begin()+l, buffer.begin()+r, a.begin()+l);
        if (log) {
            *log << "merge [" << l << ',' << r << "): ";
            for (int p=l; p<r; ++p) *log << a[p] << (p+1==r?'\n':' ');
        }
    };
    go(0, a.size());
}
inline void quickFirst(vector<int>& a, int l, int r, ostream* log = nullptr) {
    if (l >= r) return;
    int pivot=a[l], boundary=l;
    for (int i=l+1; i<=r; ++i) {
        if (a[i]<pivot) swap(a[++boundary], a[i]);
    }
    swap(a[l], a[boundary]);
    if (log) {
        *log << "partition [" << l << ',' << r << "] pivot=" << pivot << ": ";
        printVector(a, *log);
    }
    quickFirst(a, l, boundary-1, log); quickFirst(a, boundary+1, r, log);
}
inline void selectionSort(vector<int>& a) {
    for (size_t i=0; i<a.size(); ++i) {
        auto j=min_element(a.begin()+i, a.end()); iter_swap(a.begin()+i, j);
    }
}
inline void showSort(const vector<int>& input, const string& method) {
    auto a=input, expected=input; sort(expected.begin(), expected.end());
    if (method=="heap") heapSort(a, &cout);
    else if (method=="quick") quickFirst(a, 0, int(a.size())-1, &cout);
    else if (method=="merge") mergeSort(a, &cout);
    else if (method=="minheap") a=minHeapSort(a, &cout);
    else throw invalid_argument("sort method");
    assert(a==expected); cout << "sorted: "; printVector(a);
}

template<class T> void hashInsert(vector<optional<T>>& table, const T& key, int hash, int step,
                                   ostream* log = nullptr) {
    int n=table.size();
    if (n==0 || step<=0) throw invalid_argument("invalid probing parameters");
    hash=(hash%n+n)%n;
    if (log) *log << key << ":";
    for (int i=0; i<n; ++i) { // 步長不互質也不會無窮迴圈。
        int slot=(hash+1LL*i*step)%n;
        if (log) *log << (i?" -> ":" ") << slot;
        if (!table[slot]) { table[slot]=key; if (log) *log << '\n'; return; }
    }
    throw overflow_error("probing cycle or full table");
}
template<class T> void printHash(const vector<optional<T>>& table) {
    for (size_t i=0; i<table.size(); ++i) {
        cout << i << ':';
        if (table[i]) cout << *table[i]; else cout << '.';
        cout << '\n';
    }
}
inline U64 fibonacci(int n) {
    if (n<0 || n>93) throw out_of_range("uint64_t Fibonacci: 0 <= n <= 93");
    if (n==0) return 0;
    U64 previous=0, current=1;
    for (int i=2; i<=n; ++i) { U64 next=previous+current; previous=current; current=next; }
    return current;
}
inline vector<U64> pascal(int k) {
    if (k<1 || k>68) throw out_of_range("uint64_t Pascal: 1 <= k <= 68");
    vector<U64> row(k); row[0]=1;
    for (int n=1; n<k; ++n) {
        for (int j=n; j>=1; --j) row[j]+=row[j-1];
    }
    return row;
}
inline vector<string> allTrees(int height) {
    if (height<0) throw invalid_argument("negative height");
    vector<string> trees{"."};
    for (int h=1; h<=height; ++h) {
        vector<string> next{"."};
        for (const auto& l: trees) for (const auto& r: trees) next.push_back("("+l+r+")");
        trees=move(next);
    }
    return trees;
}
// 從頂點0至頂點n-1的最長簡單路徑。R[mask]以位元記錄可行終點。
inline vector<int> longestPath(const Matrix& g) {
    int n=g.size();
    if (n<1 || n>20) throw out_of_range("subset DP demonstration supports 1 <= n <= 20");
    for (const auto& row:g) if (int(row.size())!=n) throw invalid_argument("matrix shape");
    vector<uint32_t> adjacent(n);
    for (int u=0; u<n; ++u) for (int v=0; v<n; ++v) {
        if (g[u][v]!=g[v][u]) throw invalid_argument("undirected graph required");
        if (u!=v && g[u][v]) adjacent[u]|=1u<<v;
    }
    uint32_t total=1u<<n, best=0;
    vector<uint32_t> reachable(total); reachable[1]=1;
    auto popcount=[](uint32_t x) { int n=0; while (x) { x&=x-1; ++n; } return n; };
    for (uint32_t mask=1; mask<total; mask+=2) {
        if ((reachable[mask]&(1u<<(n-1))) && (!best || popcount(mask)>popcount(best))) best=mask;
        for (int u=0; u<n; ++u) if (reachable[mask]&(1u<<u)) {
            for (int v=0; v<n; ++v) if (!(mask&(1u<<v)) && (adjacent[u]&(1u<<v))) {
                reachable[mask|(1u<<v)]|=1u<<v;
            }
        }
    }
    if (!best) return {};
    vector<int> path; int last=n-1; uint32_t mask=best;
    while (mask!=1) {
        path.push_back(last); mask^=1u<<last;
        uint32_t candidates=reachable[mask]&adjacent[last];
        last=0; while (!(candidates&(1u<<last))) ++last;
    }
    path.push_back(0); reverse(path.begin(), path.end()); return path;
}
// INF是不可達的哨兵；輸入的有效最短距離須小於INF。
inline vector<long long> dijkstraMatrix(const WeightMatrix& g, int source) {
    int n=g.size();
    if (source<0 || source>=n) throw out_of_range("source");
    for (const auto& row:g) {
        if (int(row.size())!=n) throw invalid_argument("matrix shape");
        for (auto w:row) if (w<0 || w>INF) throw invalid_argument("nonnegative weights or INF only");
    }
    vector<long long> d(n,INF); vector<bool> done(n); d[source]=0;
    for (int round=0; round<n; ++round) {
        int u=-1;
        for (int v=0; v<n; ++v) if (!done[v] && (u<0 || d[v]<d[u])) u=v;
        if (u<0 || d[u]==INF) break;
        done[u]=true;
        for (int v=0; v<n; ++v) {
            if (!done[v] && g[u][v]!=INF && g[u][v]<=INF-d[u]) d[v]=min(d[v],d[u]+g[u][v]);
        }
    }
    return d;
}
inline U64 floodingMessages(const vector<vector<int>>& g) {
    U64 messages=0;
    for (int source=0; source<int(g.size()); ++source) {
        vector<bool> seen(g.size()); queue<int> q; seen[source]=true; q.push(source);
        while (!q.empty()) {
            int u=q.front(); q.pop();
            for (int v:g[u]) {
                ++messages; // 每個 origin 的廣告只在該 router 第一次收到時轉發一次。
                if (!seen[v]) { seen[v]=true; q.push(v); }
            }
        }
    }
    return messages;
}
inline vector<vector<int>> lcsTable(const string& x, const string& y) {
    vector<vector<int>> d(x.size()+1,vector<int>(y.size()+1));
    for (size_t i=1; i<=x.size(); ++i) for (size_t j=1; j<=y.size(); ++j) {
        d[i][j]=x[i-1]==y[j-1] ? d[i-1][j-1]+1 : max(d[i-1][j],d[i][j-1]);
    }
    return d;
}
inline string lcsString(const string& x, const string& y, const vector<vector<int>>& d) {
    int i=x.size(), j=y.size(); string answer;
    while (i && j) {
        if (x[i-1]==y[j-1]) { answer+=x[--i]; --j; }
        else if (d[i-1][j]>=d[i][j-1]) --i;
        else --j;
    }
    reverse(answer.begin(),answer.end()); return answer;
}
inline int balancedSplit(const string& text) {
    int balance=0, pieces=0;
    for (char c:text) {
        if (c!='L' && c!='R') throw invalid_argument("L/R only");
        balance+=c=='L'?1:-1;
        if (balance==0) ++pieces;
    }
    if (balance!=0) throw invalid_argument("input is not balanced");
    return pieces;
}
inline pair<U64,U64> avlNodeBounds(int edgeHeight) {
    if (edgeHeight<0 || edgeHeight>62) throw out_of_range("height");
    U64 previous=0, current=1;
    for (int h=1; h<=edgeHeight; ++h) { U64 next=1+previous+current; previous=current; current=next; }
    return {(U64(1)<<(edgeHeight+1))-1,current};
}
inline vector<int> bfs(const vector<vector<int>>& g, int source) {
    vector<bool> seen(g.size()); vector<int> out; queue<int> q;
    seen.at(source)=true; q.push(source);
    while (!q.empty()) {
        int u=q.front(); q.pop(); out.push_back(u);
        for (int v:g[u]) if (!seen.at(v)) { seen[v]=true; q.push(v); }
    }
    return out;
}
inline int sequential(const vector<int>& a, int target) {
    for (int i=0; i<int(a.size()); ++i) if (a[i]==target) return i;
    return -1;
}
inline int binary(const vector<int>& a, int target) {
    int l=0,r=a.size();
    while (l<r) {
        int m=l+(r-l)/2;
        if (a[m]==target) return m;
        if (a[m]<target) l=m+1; else r=m;
    }
    return -1;
}

inline int precedence(const string& op) {
    if (op=="<") return 1;
    if (op=="+" || op=="-") return 2;
    if (op=="*" || op=="/") return 3;
    return 0;
}
inline vector<string> tokenize(const string& text) {
    vector<string> tokens;
    for (size_t i=0; i<text.size();) {
        char c=text[i];
        if (c==' ') { ++i; continue; }
        if (c>='0' && c<='9') {
            size_t begin=i;
            while (i<text.size() && text[i]>='0' && text[i]<='9') ++i;
            tokens.push_back(text.substr(begin,i-begin));
        } else {
            string t(1,c);
            if (!precedence(t) && c!='(' && c!=')') throw invalid_argument("token");
            tokens.push_back(t); ++i;
        }
    }
    return tokens;
}
inline Ptr<string> parseExpression(const string& text) {
    vector<string> operators; vector<Ptr<string>> operands;
    auto reduce=[&]() {
        if (operators.empty() || operands.size()<2) throw invalid_argument("missing operand");
        auto right=move(operands.back()); operands.pop_back();
        auto left=move(operands.back()); operands.pop_back();
        auto op=operators.back(); operators.pop_back();
        operands.push_back(node<string>(op,move(left),move(right)));
    };
    for (const auto& t:tokenize(text)) {
        if (t=="(") operators.push_back(t);
        else if (t==")") {
            while (!operators.empty() && operators.back()!="(") reduce();
            if (operators.empty()) throw invalid_argument("parenthesis");
            operators.pop_back();
        } else if (precedence(t)) {
            while (!operators.empty() && precedence(operators.back())>=precedence(t)) reduce();
            operators.push_back(t);
        } else operands.push_back(node<string>(t));
    }
    while (!operators.empty()) {
        if (operators.back()=="(") throw invalid_argument("parenthesis");
        reduce();
    }
    if (operands.size()!=1) throw invalid_argument("expression");
    return move(operands[0]);
}
inline double evaluate(const Node<string>* p) {
    if (!p) throw invalid_argument("empty expression");
    if (!p->left && !p->right) return stod(p->key);
    double a=evaluate(p->left.get()), b=evaluate(p->right.get());
    if (p->key=="+") return a+b;
    if (p->key=="-") return a-b;
    if (p->key=="*") return a*b;
    if (p->key=="/") { if (b==0) throw domain_error("division by zero"); return a/b; }
    if (p->key=="<") return a<b;
    throw invalid_argument("operator");
}
inline void printExpression(const Node<string>* p, ostream& out = cout) {
    if (!p) throw invalid_argument("empty expression");
    if (!p->left && !p->right) { out<<p->key; return; }
    out<<'('; printExpression(p->left.get(),out); out<<p->key;
    printExpression(p->right.get(),out); out<<')';
}
inline Ptr<char> letterTree112() {
    return node<char>('E',node<char>('H',node<char>('D'),node<char>('B',node<char>('F'))),
                      node<char>('G',{},node<char>('A')));
}
inline Ptr<char> letterTree111() {
    return node<char>('+',node<char>('*',node<char>('-',node<char>('*',node<char>('A'),node<char>('B')),node<char>('C')),node<char>('D')),node<char>('E'));
}
inline void protocolChoice() {
    // 題目是協定概念判断；矩陣演算法僅示範 A 的最短路徑部分。
    WeightMatrix g{{0,1,4},{INF,0,2},{INF,INF,0}};
    assert(dijkstraMatrix(g,0)==vector<long long>({0,1,3}));
    constexpr array<bool,5> trueStatement{true,false,false,true,false};
    cout<<"true statements: ";
    for (int i=0;i<5;++i) if (trueStatement[i]) cout<<char('A'+i);
    cout<<"; choice=3; example shortest distance=3\n";
}
} // namespace nccu
