#include "../solutions/nccu/common.hpp"
using namespace nccu;

int bruteLongest(const Matrix& g) {
    int n=g.size(),best=0;
    vector<bool> seen(n); seen[0]=true;
    function<void(int,int)> dfs=[&](int u,int length) {
        if (u==n-1) { best=max(best,length); return; }
        for (int v=0;v<n;++v) if (g[u][v]&&!seen[v]) {
            seen[v]=true; dfs(v,length+1); seen[v]=false;
        }
    };
    dfs(0,1); return best;
}
int parseShape(const string& text,size_t& at) {
    assert(at<text.size());
    if (text[at++]=='.') return 0;
    assert(text[at-1]=='(');
    int l=parseShape(text,at),r=parseShape(text,at);
    assert(text.at(at++)==')'); return 1+max(l,r);
}
bool isSubsequence(const string& a,const string& b) {
    size_t i=0;
    for (char c:b) if (i<a.size()&&a[i]==c) ++i;
    return i==a.size();
}
int bruteLCS(const string& a,const string& b) {
    int best=0;
    for (unsigned mask=0;mask<(1u<<a.size());++mask) {
        string s;
        for (size_t i=0;i<a.size();++i) if (mask&(1u<<i)) s+=a[i];
        if (isSubsequence(s,b)) best=max(best,int(s.size()));
    }
    return best;
}
int dpSplit(const string& s) {
    vector<int> d(s.size()+1,-1000);d[0]=0;
    for (int r=1;r<=int(s.size());++r) {
        int balance=0;
        for (int l=r-1;l>=0;--l) {
            balance+=s[l]=='L'?1:-1;
            if (balance==0) d[r]=max(d[r],d[l]+1);
        }
    }
    return d.back();
}

int main() {
    mt19937 gen(20261013);
    int cases=0;
    // Heap、merge、first-pivot quick 與標準排序做差分比較。
    for (int n=0;n<=64;++n) for (int t=0;t<4;++t) {
        vector<int> a(n);
        for (int& x:a) x=int(gen()%31)-15;
        auto expected=a;sort(expected.begin(),expected.end());
        for (int method=0;method<4;++method) {
            auto b=a;
            if (method==0) heapSort(b);
            if (method==1) mergeSort(b);
            if (method==2) quickFirst(b,0,int(b.size())-1);
            if (method==3) selectionSort(b);
            assert(b==expected);++cases;
        }
        assert(minHeapSort(a)==expected);++cases;
    }
    // LL、RR、LR、RL 的固定反例與插刪隨機序列。
    for (auto values:vector<vector<int>>{{3,2,1},{1,2,3},{3,1,2},{1,3,2}}) {
        Ptr<int> tree;for (int x:values) avlInsert(tree,x);
        assert(tree->key==2);checkAVL(tree.get());++cases;
    }
    Ptr<int> avl;set<int> expected;
    for (int t=0;t<1200;++t) {
        int x=gen()%150;
        if (gen()%2) {avlInsert(avl,x);expected.insert(x);}
        else {avlErase(avl,x);expected.erase(x);}
        assert(traversal(avl,1)==vector<int>(expected.begin(),expected.end()));
        checkAVL(avl.get());++cases;
    }
    // Splay 插入前的樹確實為原题 AVL；操作後仍保持全局 BST 順序。
    Ptr<int> splayTree;
    for (int x:keys112) avlInsert(splayTree,x);
    auto keys=keys112;keys.push_back(118);sort(keys.begin(),keys.end());
    bstInsert(splayTree,118);splayTree=splay(move(splayTree),118);
    assert(splayTree->key==118 && traversal(splayTree,1)==keys);++cases;
    splayTree=splay(move(splayTree),22);
    assert(splayTree->key==22 && traversal(splayTree,1)==keys);++cases;
    // 窮舉 DFS 是 subset DP 的獨立 oracle，亦檢查還原路徑。
    for (int n=1;n<=8;++n) for (int t=0;t<40;++t) {
        Matrix g(n,vector<bool>(n));
        for (int u=0;u<n;++u) for (int v=u+1;v<n;++v) g[u][v]=g[v][u]=gen()%2;
        auto path=longestPath(g);assert(int(path.size())==bruteLongest(g));
        if (!path.empty()) {
            assert(path.front()==0&&path.back()==n-1);
            assert(set<int>(path.begin(),path.end()).size()==path.size());
            for (size_t i=1;i<path.size();++i) assert(g[path[i-1]][path[i]]);
        }
        ++cases;
    }
    // 最短路徑以 Floyd-Warshall 比對，包括不連通圖。
    for (int t=0;t<80;++t) {
        int n=7;WeightMatrix g(n,vector<long long>(n,INF));
        for (int u=0;u<n;++u) {
            g[u][u]=0;
            for (int v=0;v<n;++v) if (u!=v&&gen()%4==0) g[u][v]=gen()%20;
        }
        auto d=g;
        for (int k=0;k<n;++k) for (int i=0;i<n;++i) for (int j=0;j<n;++j) {
            if (d[i][k]!=INF&&d[k][j]!=INF) d[i][j]=min(d[i][j],d[i][k]+d[k][j]);
        }
        for (int source=0;source<n;++source) {assert(dijkstraMatrix(g,source)==d[source]);++cases;}
    }
    // LCS 回傳的字串須同時為兩輸入的子序列，且長度等於窮舉最佳值。
    for (int t=0;t<100;++t) {
        string a(7,'a'),b(7,'a');
        for (char& c:a) c='a'+gen()%3;
        for (char& c:b) c='a'+gen()%3;
        auto d=lcsTable(a,b);auto s=lcsString(a,b,d);
        assert(isSubsequence(s,a)&&isSubsequence(s,b)&&int(s.size())==bruteLCS(a,b));++cases;
    }
    assert(lcsTable("","abc").back().back()==0);++cases;
    array<int,5> counts{1,2,5,26,677};
    for (int h=0;h<=4;++h) {
        auto shapes=allTrees(h);assert(int(shapes.size())==counts[h]);
        assert(set<string>(shapes.begin(),shapes.end()).size()==shapes.size());
        for (auto& shape:shapes) {size_t at=0;assert(parseShape(shape,at)<=h&&at==shape.size());++cases;}
    }
    for (int n=1;n<=6;++n) for (int t=0;t<30;++t) {
        string s(n,'L');s+=string(n,'R');shuffle(s.begin(),s.end(),gen);
        assert(balancedSplit(s)==dpSplit(s));++cases;
    }
    for (int n=0;n<50;++n) {
        vector<int> a(n);
        for (int& x:a) x=int(gen()%20)-10;
        sort(a.begin(),a.end());
        for (int target=-12;target<=12;++target) {
            int found=binary(a,target);
            bool exists=binary_search(a.begin(),a.end(),target);
            assert((found>=0)==exists);
            if (found>=0) assert(a[found]==target);
            ++cases;
        }
    }
    // 不互質的 double-hash 步長不得造成無窮迴圈。
    vector<optional<int>> table(6);table[0]=0;table[2]=2;table[4]=4;
    bool rejected=false;
    try {hashInsert(table,6,0,2);} catch (const overflow_error&) {rejected=true;}
    assert(rejected && !table[1]);++cases;
    assert(StringLess{}("\"A","AA") && StringLess{}("Z","AA") && !StringLess{}("Aa","AA"));++cases;
    assert(pascal(68)[33]==14226520737620288370ULL);++cases;
    for (const auto& text:vector<string>{"1+2*3","(1+2)*3","9*(3*5)-28/(8-1)<105+2*(3+4)-18","20/(2+3)"}) {
        auto p=parseExpression(text);ostringstream out;printExpression(p.get(),out);
        auto q=parseExpression(out.str());assert(evaluate(p.get())==evaluate(q.get()));++cases;
    }
    assert(evaluate(parseExpression("1+2*3").get())==7);++cases;
    assert(evaluate(parseExpression("(1+2)*3").get())==9);++cases;
    cout<<"PASS "<<cases<<" property/differential cases\n";
}
