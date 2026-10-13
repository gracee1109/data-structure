#include "../solutions/nycu/common.hpp"
using namespace nycu;

struct Tagged {
    int key=0,position=0;
    bool operator==(const Tagged& b) const {return key==b.key && position==b.position;}
};
std::ostream& operator<<(std::ostream& out,const Tagged& value) {return out<<value.key<<':'<<value.position;}
Cost bruteMST(int n,const vector<Edge>& e) {
    Cost best=INF;
    for (unsigned mask=0;mask<(1u<<e.size());++mask) {
        unsigned bits=mask;int count=0;
        while (bits) {bits&=bits-1;++count;}
        if (count!=n-1) continue;
        vector<int> parent(n);iota(parent.begin(),parent.end(),0);
        function<int(int)> find=[&](int x) {return parent[x]==x?x:parent[x]=find(parent[x]);};
        Cost sum=0;bool valid=true;
        for (size_t i=0;i<e.size();++i) if (mask&(1u<<i)) {
            int a=find(e[i].u),b=find(e[i].v);
            if (a==b) {valid=false;break;}
            parent[a]=b;sum+=e[i].w;
        }
        if (valid) best=min(best,sum); // n-1條無環邊自動連通。
    }
    return best;
}
vector<Cost> exhaustiveDistances(const Matrix& g,int source) {
    int n=g.size();vector<bool> seen(n);vector<Cost> best(n,INF);seen[source]=true;
    function<void(int,Cost)> dfs=[&](int u,Cost cost) {
        best[u]=min(best[u],cost);
        for (int v=0;v<n;++v) if (!seen[v] && g[u][v]!=INF) {
            seen[v]=true;dfs(v,cost+g[u][v]);seen[v]=false;
        }
    };
    dfs(source,0);return best;
}
void validCycle(const Graph& g,const optional<Cycle>& cycle) {
    if (!cycle) return;
    set<int> vertices(cycle->begin(),cycle->end());assert(vertices.size()==4);
    for (int i=0;i<4;++i) assert(g[(*cycle)[i]][(*cycle)[(i+1)%4]]);
}
int main() {
    U64 cases=0;mt19937 gen(20261013);
    // 最短路徑用窮舉所有簡單路徑比對，含負邊的測試圖限定DAG。
    for (bool negative:{false,true}) for (int n=1;n<=6;++n) for (int trial=0;trial<15;++trial) {
        vector<Edge> edges;
        for (int u=0;u<n;++u) for (int v=0;v<n;++v) if (u!=v && (!negative || u<v) && gen()%3==0) {
            edges.push_back({u,v,negative?Cost(int(gen()%20)-8):Cost(gen()%20)});
        }
        auto g=matrix(n,edges),f=floyd(g),bounded=boundedAPSP(g,n-1);
        assert(f==bounded);
        for (int s=0;s<n;++s) {
            auto truth=exhaustiveDistances(g,s);auto bf=bellmanFord(n,edges,s);
            assert(f[s]==truth && bf.distance==truth);
            if (!negative) assert(dijkstra(g,s).distance==truth);
            for (int t=0;t<n;++t) if (truth[t]!=INF) {
                auto path=restore(bf,s,t);assert(path.front()==s && path.back()==t && pathWeight(g,path)==truth[t]);
            }
            ++cases;
        }
    }
    Matrix boundedExample=matrix(3,{{0,1,2},{1,2,3},{0,2,9}});
    assert(boundedAPSP(boundedExample,1)[0][2]==9 && boundedAPSP(boundedExample,2)[0][2]==5);++cases;
    bool rejected=false;
    try {(void)bellmanFord(3,{{0,1,1},{1,2,-2},{2,1,-2}},0);} catch (const domain_error&) {rejected=true;}
    assert(rejected);++cases;
    assert(bellmanFord(3,{{1,2,-2},{2,1,-2}},0).distance==vector<Cost>({0,INF,INF}));++cases;
    assert(addCost(INF,-5)==INF);++cases;
    rejected=false;try {(void)addCost(INF-1,1);}catch (const overflow_error&) {rejected=true;}
    assert(rejected);++cases;
    // 原卷两張圖的不同邊權不能混用；反例必須實際跑出1與2的差異。
    auto positive=matrix(6,graph114(false)),negative=matrix(6,graph114(true));
    assert(positive[3][2]==1 && negative[3][2]==2 && positive[2][1]==INF && negative[2][1]==-2);++cases;
    assert(bfs(positive,5).distance[1]==2 && pathWeight(positive,restore(bfs(positive,5),5,1))==3);++cases;
    assert(dijkstra(positive,5).distance[1]==2);++cases;
    assert(frozenDijkstraForCounterexample(negative,5).distance[1]==2 && bellmanFord(6,graph114(true),5).distance[1]==1);++cases;
    // MST以枚舉n-1條邊的所有子集作獨立oracle。
    assert(bruteMST(6,mst115)==15);++cases;
    auto original=matrix(6,mst115,true);
    for (int root=0;root<6;++root) {auto tree=prim(original,root);assert(tree && tree->weight==15 && edgeSet(*tree)==edgeSet(*prim(original,0)));++cases;}
    for (int n=1;n<=5;++n) for (int trial=0;trial<25;++trial) {
        vector<Edge> e;
        for (int u=0;u<n;++u) for (int v=u+1;v<n;++v) if (v==u+1 || gen()%2) e.push_back({u,v,Cost(int(gen()%20)-5)});
        Cost expected=bruteMST(n,e);auto g=matrix(n,e,true);
        for (int s=0;s<n;++s) {auto t=prim(g,s);assert(t && t->weight==expected && int(t->edges.size())==n-1);++cases;}
    }
    assert(!prim(matrix(3,{{0,1,1}},true),0));assert(prim({},0)->weight==0);++cases;
    // 子集合計數直接枚舉位元遮罩；不重用二項式公式。
    for (int n=0;n<=16;n+=2) {
        U64 count=0;
        for (unsigned mask=0;mask<(1u<<n);++mask) {
            unsigned bits=mask;int size=0;while (bits) {bits&=bits-1;++size;}
            if (size<=n/2) ++count;
        }
        assert(smallSubsets(n)==count);++cases;
    }
    assert(smallSubsets(62)==2538557185841324496ULL);++cases;
    // 實際七分支遞迴樹與獨立代數解比較，n=2^k。
    U64 seven=7,four=4;
    for (int n=1;n<=64;n*=2) {
        auto t=recurrenceTree(n);assert(t.work==(seven-four)/3 && t.calls==(seven-1)/6);
        seven*=7;four*=4;++cases;
    }
    // 雜湊碰撞、滿表、重複鍵與負鍵都用set比較。
    for (int m=1;m<=9;++m) {
        Chaining chain(m);LinearHash linear(m);set<int> chainRef,linearRef;
        for (int trial=0;trial<70;++trial) {
            int key=int(gen()%31)-15;chain.insert(key);chainRef.insert(key);
            bool canPlace=linearRef.count(key) || int(linearRef.size())<m;
            assert(linear.insert(key)==canPlace);if (canPlace) linearRef.insert(key);
            for (int target=-16;target<=16;++target) {
                assert(chain.contains(target)==bool(chainRef.count(target)));
                assert(linear.contains(target)==bool(linearRef.count(target)));
            }
            ++cases;
        }
    }
    rejected=false;try {Chaining tooBig(numeric_limits<int>::max());}catch (const invalid_argument&) {rejected=true;}
    assert(rejected);++cases;
    // 帶原始位置的同鍵資料，比對stable_sort以驗證穩定合併。
    auto lessKey=[](const Tagged& a,const Tagged& b){return a.key<b.key;};
    for (int n=0;n<=60;++n) {
        vector<Tagged> items(n);for (int i=0;i<n;++i) items[i]={int(gen()%8),i};
        auto expected=items;stable_sort(expected.begin(),expected.end(),lessKey);mergeSort(items,lessKey);assert(items==expected);++cases;
        vector<int> a(n),b(n+1);for (int& x:a) x=int(gen()%30)-15;for (int& x:b) x=int(gen()%30)-15;
        sort(a.begin(),a.end());sort(b.begin(),b.end());auto combined=a;combined.insert(combined.end(),b.begin(),b.end());sort(combined.begin(),combined.end());
        U64 comparisons=0;assert(mergeLists(a,b,less<int>{},&comparisons)==combined && comparisons<=a.size()+b.size());++cases;
    }
    for (int n=0;n<=15;++n) {assert(stairsRecursive(n).first==stairsDP(n));++cases;}
    assert(stairsDP(73)==12903063846126135669ULL);++cases;
    rejected=false;try {(void)stairsDP(74);}catch (const invalid_argument&) {rejected=true;}assert(rejected);++cases;
    // 全部65536組8-bit輸入，以逐位布林邏輯驗證位元運算。
    for (int a=0;a<256;++a) for (int b=0;b<256;++b) {
        auto actual=logic8(a,b);array<unsigned,3> expected{};
        for (int bit=0;bit<8;++bit) {
            bool x=(a>>bit)&1,y=(b>>bit)&1;
            if (x && y) expected[0]+=1u<<bit;
            if (x || y) expected[1]+=1u<<bit;
            if (x!=y) expected[2]+=1u<<bit;
        }
        for (int i=0;i<3;++i) { assert(actual[i]==expected[i]); }
        ++cases;
    }
    // 窮舉6頂點以下全部簡單無向圖，比對四組合版和鄰居對版。
    for (int n=0;n<=6;++n) {
        vector<pair<int,int>> positions;
        for (int u=0;u<n;++u) for (int v=u+1;v<n;++v) positions.emplace_back(u,v);
        for (unsigned mask=0;mask<(1u<<positions.size());++mask) {
            Graph g(n,vector<bool>(n));
            for (size_t i=0;i<positions.size();++i) if (mask&(1u<<i)) {auto [u,v]=positions[i];g[u][v]=g[v][u]=true;}
            U64 pairs=0;auto brute=fourCycleBrute(g),fast=fourCycleFast(g,&pairs);
            assert(bool(brute)==bool(fast));validCycle(g,brute);validCycle(g,fast);
            assert(pairs<=U64(n)*U64(n? n-1:0)/2+1);++cases;
        }
    }
    for (int n=7;n<=10;++n) for (int trial=0;trial<50;++trial) {
        Graph g(n,vector<bool>(n));
        for (int u=0;u<n;++u) for (int v=u+1;v<n;++v) g[u][v]=g[v][u]=gen()%2;
        U64 pairs=0;auto a=fourCycleBrute(g),b=fourCycleFast(g,&pairs);
        assert(bool(a)==bool(b));validCycle(g,b);assert(pairs<=U64(n)*(n-1)/2+1);++cases;
    }
    for (int n=2;n<=100;++n) {
        Graph star(n,vector<bool>(n));for (int v=1;v<n;++v) star[0][v]=star[v][0]=true;
        U64 pairs=0;assert(!fourCycleFast(star,&pairs) && pairs==U64(n-1)*(n-2)/2);++cases;
    }
    cout<<"PASS "<<cases<<" property/differential cases\n";
}
