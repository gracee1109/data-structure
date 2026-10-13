#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace nycu {
using namespace std;
using U64=uint64_t;
using Cost=long long;
using Matrix=vector<vector<Cost>>;
using Graph=vector<vector<bool>>;
inline constexpr Cost INF=Cost(1)<<60;
struct Edge { int u,v; Cost w; }; // 內部頂點編號從0起。

inline Cost addCost(Cost a,Cost b) {
    if (a==INF || b==INF) return INF;
    if ((b>0 && a>=INF-b) || (b<0 && a<=-INF-b)) throw overflow_error("finite distance must be between -INF and INF");
    return a+b;
}
inline U64 addCount(U64 a,U64 b) {
    if (a>numeric_limits<U64>::max()-b) throw overflow_error("uint64_t count overflow");
    return a+b;
}
template<class T> void printVector(const vector<T>& a,ostream& out=cout) {
    for (size_t i=0;i<a.size();++i) { if (i) out<<' '; out<<a[i]; }
    out<<'\n';
}
inline void printMatrix(const Matrix& a) {
    for (auto& row:a) {
        for (size_t j=0;j<row.size();++j) {
            if (j) cout<<' ';
            if (row[j]==INF) cout<<"INF"; else cout<<row[j];
        }
        cout<<'\n';
    }
}
inline void validate(const Matrix& g) {
    for (const auto& row:g) {
        if (row.size()!=g.size()) throw invalid_argument("square matrix required");
        for (Cost w:row) if (w!=INF && (w<=-INF || w>=INF)) throw invalid_argument("weight range");
    }
}
inline Matrix matrix(int n,const vector<Edge>& edges,bool undirected=false) {
    if (n<0) throw invalid_argument("negative vertex count");
    Matrix g(n,vector<Cost>(n,INF));
    for (int i=0;i<n;++i) g[i][i]=0;
    for (auto [u,v,w]:edges) {
        if (u<0 || u>=n || v<0 || v>=n || w<=-INF || w>=INF) throw invalid_argument("edge");
        g[u][v]=min(g[u][v],w);
        if (undirected) g[v][u]=min(g[v][u],w);
    }
    return g;
}
// f(i,j,r): 至多r條邊。next只能讀前一輪，不能在同輪連鎖更新。
inline Matrix boundedAPSP(const Matrix& g,int maxEdges) {
    validate(g);
    if (maxEdges<0) throw invalid_argument("edge bound");
    int n=g.size(); Matrix d(n,vector<Cost>(n,INF));
    for (int i=0;i<n;++i) d[i][i]=0;
    for (int r=1;r<=maxEdges;++r) {
        Matrix next=d;
        for (int i=0;i<n;++i) for (int j=0;j<n;++j) for (int k=0;k<n;++k) {
            next[i][j]=min(next[i][j],addCost(d[i][k],g[k][j]));
        }
        d=move(next);
    }
    return d;
}
inline Matrix floyd(const Matrix& g) {
    validate(g); auto d=g; int n=d.size();
    for (int i=0;i<n;++i) d[i][i]=min(d[i][i],Cost(0));
    for (int k=0;k<n;++k) for (int i=0;i<n;++i) for (int j=0;j<n;++j) {
        d[i][j]=min(d[i][j],addCost(d[i][k],d[k][j]));
    }
    return d; // 若d[k][k]<0，有負環，相關點對沒有有限最短路徑。
}
struct Paths { vector<Cost> distance; vector<int> parent,order; };
// 僅供第14題反例：刻意保留「一旦定案，絕不修改」以呈現負邊導致的錯誤。
inline Paths frozenDijkstraForCounterexample(const Matrix& g,int source) {
    validate(g); int n=g.size();
    if (source<0 || source>=n) throw out_of_range("source");
    Paths out{vector<Cost>(n,INF),vector<int>(n,-1),{}};
    vector<bool> done(n); out.distance[source]=0;
    for (int round=0;round<n;++round) {
        int u=-1;
        for (int v=0;v<n;++v) if (!done[v] && (u<0 || out.distance[v]<out.distance[u])) u=v;
        if (u<0 || out.distance[u]==INF) break;
        done[u]=true; out.order.push_back(u);
        for (int v=0;v<n;++v) if (!done[v]) {
            Cost candidate=addCost(out.distance[u],g[u][v]);
            if (candidate<out.distance[v]) {out.distance[v]=candidate;out.parent[v]=u;}
        }
    }
    return out;
}
inline Paths dijkstra(const Matrix& g,int source) {
    validate(g);
    for (const auto& row:g) for (Cost w:row) if (w<0) throw invalid_argument("Dijkstra requires nonnegative edges");
    return frozenDijkstraForCounterexample(g,source);
}
inline Paths bellmanFord(int n,const vector<Edge>& edges,int source) {
    if (n<=0 || source<0 || source>=n) throw invalid_argument("vertices/source");
    for (auto e:edges) if (e.u<0 || e.u>=n || e.v<0 || e.v>=n || e.w<=-INF || e.w>=INF) throw invalid_argument("edge");
    Paths out{vector<Cost>(n,INF),vector<int>(n,-1),{}}; out.distance[source]=0;
    for (int r=1;r<n;++r) {
        bool changed=false;
        for (auto e:edges) {
            Cost candidate=addCost(out.distance[e.u],e.w);
            if (candidate<out.distance[e.v]) {out.distance[e.v]=candidate;out.parent[e.v]=e.u;changed=true;}
        }
        if (!changed) break;
    }
    for (auto e:edges) if (addCost(out.distance[e.u],e.w)<out.distance[e.v]) throw domain_error("reachable negative cycle");
    return out;
}
inline vector<int> restore(const Paths& paths,int source,int target) {
    int n=paths.distance.size();
    if (source<0 || source>=n || target<0 || target>=n || int(paths.parent.size())!=n) throw invalid_argument("path bounds");
    if (paths.distance[target]==INF) return {};
    vector<int> path;
    for (int v=target;v!=-1;v=paths.parent[v]) {
        if (int(path.size())>=n) throw logic_error("parent cycle");
        path.push_back(v);
        if (v==source) {reverse(path.begin(),path.end());return path;}
    }
    throw logic_error("parent chain missing source");
}
inline void printPath(vector<int> path) {
    for (int& v:path) ++v;
    printVector(path);
}
inline Paths bfs(const Matrix& g,int source) {
    validate(g); int n=g.size();
    if (source<0 || source>=n) throw out_of_range("source");
    Paths out{vector<Cost>(n,INF),vector<int>(n,-1),{}};queue<int> q;
    out.distance[source]=0;q.push(source);
    while (!q.empty()) {
        int u=q.front();q.pop();out.order.push_back(u);
        for (int v=0;v<n;++v) if (u!=v && g[u][v]!=INF && out.distance[v]==INF) {
            out.distance[v]=out.distance[u]+1;out.parent[v]=u;q.push(v);
        }
    }
    return out; // distance表示hop，不是原圖權重。
}
inline Cost pathWeight(const Matrix& g,const vector<int>& path) {
    Cost sum=0;
    for (size_t i=1;i<path.size();++i) {
        Cost w=g.at(path[i-1]).at(path[i]);
        if (w==INF) throw invalid_argument("missing path edge");
        sum=addCost(sum,w);
    }
    return sum;
}
inline vector<Edge> graph114(bool negative) {
    vector<Edge> e{{5,3,1},{5,4,1},{3,2,negative?2:1},{3,1,2},{4,3,1},{4,1,1},{4,0,1},{1,0,1}};
    if (negative) e.push_back({2,1,-2});
    return e;
}
inline const vector<Edge> mst115{{3,2,1},{3,1,2},{3,5,3},{3,4,4},{4,0,5},{4,1,6},{1,0,7},{5,4,8}};
struct MST { Cost weight=0;vector<Edge> edges; };
inline optional<MST> prim(const Matrix& g,int root) {
    validate(g);int n=g.size();
    if (!n) return MST{};
    if (root<0 || root>=n) throw out_of_range("root");
    for (int u=0;u<n;++u) for (int v=0;v<n;++v) if (g[u][v]!=g[v][u]) throw invalid_argument("undirected graph required");
    vector<Cost> key(n,INF);vector<int> parent(n,-1);vector<bool> used(n);key[root]=0;MST result;
    for (int step=0;step<n;++step) {
        int u=-1;
        for (int v=0;v<n;++v) if (!used[v] && (u<0 || key[v]<key[u])) u=v;
        if (u<0 || key[u]==INF) return nullopt;
        used[u]=true;
        if (parent[u]>=0) {result.weight=addCost(result.weight,key[u]);result.edges.push_back({parent[u],u,key[u]});}
        for (int v=0;v<n;++v) if (!used[v] && g[u][v]<key[v]) {key[v]=g[u][v];parent[v]=u;}
    }
    return result;
}
inline set<pair<int,int>> edgeSet(const MST& mst) {
    set<pair<int,int>> result;
    for (auto e:mst.edges) result.emplace(min(e.u,e.v),max(e.u,e.v));
    return result;
}
inline void printMST(const MST& mst,int root) {
    cout<<"root="<<root+1<<'\n';
    for (auto e:mst.edges) cout<<e.u+1<<'-'<<e.v+1<<" weight="<<e.w<<'\n';
    cout<<"total="<<mst.weight<<'\n';
}

inline U64 smallSubsets(int n) {
    if (n<0 || n>62 || n%2) throw invalid_argument("even n, 0 <= n <= 62");
    vector<U64> choose(n+1);choose[0]=1;
    for (int row=1;row<=n;++row) for (int k=row;k>=1;--k) choose[k]=addCount(choose[k],choose[k-1]);
    return ((U64(1)<<n)+choose[n/2])/2;
}
struct RecurrenceSample { U64 work=0,calls=0; };
inline RecurrenceSample recurrenceTree(int n) {
    if (n<1 || n>64 || (n&(n-1))) throw invalid_argument("demo n is a power of 2, <=64");
    if (n==1) return {1,1};
    RecurrenceSample out{0,1};
    for (int branch=0;branch<7;++branch) {
        auto child=recurrenceTree(n/2);out.work+=child.work;out.calls+=child.calls;
    }
    for (int i=0;i<n;++i) for (int j=0;j<n;++j) ++out.work;
    return out; // 以n²代表原題的Theta(n²)非遞迴工作。
}
inline size_t hashCapacity(int m) {
    if (m<=0 || m>numeric_limits<int>::max()/2) throw invalid_argument("capacity");
    return size_t(m);
}
class Chaining {
    vector<list<int>> buckets;
    size_t hash(int key) const {int m=buckets.size();return (key%m+m)%m;}
public:
    explicit Chaining(int m):buckets(hashCapacity(m)) {}
    bool contains(int key) const {auto& b=buckets[hash(key)];return find(b.begin(),b.end(),key)!=b.end();}
    void insert(int key) {if (!contains(key)) buckets[hash(key)].push_back(key);}
    void print() const {for (size_t i=0;i<buckets.size();++i) {cout<<i<<':';for (int k:buckets[i]) cout<<' '<<k;cout<<'\n';}}
};
class LinearHash {
    vector<optional<int>> slots;
    int hash(int key) const {int m=slots.size();return (key%m+m)%m;}
public:
    explicit LinearHash(int m):slots(hashCapacity(m)) {}
    bool contains(int key) const {
        for (int i=0;i<int(slots.size());++i) {
            auto& cell=slots[(hash(key)+i)%slots.size()];
            if (!cell) return false;
            if (*cell==key) return true;
        }
        return false;
    }
    bool insert(int key) {
        for (int i=0;i<int(slots.size());++i) {
            auto& cell=slots[(hash(key)+i)%slots.size()];
            if (!cell) {cell=key;return true;}
            if (*cell==key) return true;
        }
        return false;
    }
    void print() const {for (size_t i=0;i<slots.size();++i) {cout<<i<<':';if (slots[i]) cout<<*slots[i];else cout<<'.';cout<<'\n';}}
};

template<class T,class Less=less<T>> vector<T> mergeLists(const vector<T>& left,const vector<T>& right,Less lessThan={},U64* comparisons=nullptr) {
    vector<T> out;out.reserve(left.size()+right.size());size_t i=0,j=0;
    while (i<left.size() && j<right.size()) {
        if (comparisons) ++*comparisons;
        if (!lessThan(right[j],left[i])) out.push_back(left[i++]);
        else out.push_back(right[j++]);
    }
    out.insert(out.end(),left.begin()+i,left.end());out.insert(out.end(),right.begin()+j,right.end());return out;
}
template<class T,class Less=less<T>> void mergeSort(vector<T>& a,Less lessThan={},ostream* log=nullptr) {
    vector<T> buffer(a.size());
    function<void(size_t,size_t)> go=[&](size_t l,size_t r) {
        if (r-l<2) return;
        size_t mid=l+(r-l)/2;go(l,mid);go(mid,r);size_t i=l,j=mid,k=l;
        while (i<mid || j<r) {
            if (j==r || (i<mid && !lessThan(a[j],a[i]))) buffer[k++]=a[i++];
            else buffer[k++]=a[j++];
        }
        copy(buffer.begin()+l,buffer.begin()+r,a.begin()+l);
        if (log) {
            *log<<"merge ["<<l<<','<<r<<"):";
            for (size_t p=l;p<r;++p) *log<<' '<<a[p];
            *log<<'\n';
        }
    };
    go(0,a.size());
}
inline U64 stairsDP(int n) {
    if (n<0 || n>73) throw invalid_argument("uint64_t stairs: 0 <= n <= 73");
    array<U64,3> start{1,1,2};
    if (n<=2) return start[n];
    U64 a=1,b=1,c=2;
    for (int i=3;i<=n;++i) {U64 next=addCount(addCount(a,b),c);a=b;b=c;c=next;}
    return c;
}
inline pair<U64,U64> stairsRecursive(int n) {
    if (n<0 || n>24) throw invalid_argument("unmemoized demo: 0 <= n <= 24");
    U64 calls=0;
    function<U64(int)> go=[&](int k)->U64 {
        ++calls;
        if (k<0) return 0;
        if (k==0) return 1;
        return go(k-1)+go(k-2)+go(k-3);
    };
    U64 ways=go(n);return {ways,calls};
}
inline array<uint8_t,3> logic8(uint8_t a,uint8_t b) {
    return {uint8_t(a&b),uint8_t(a|b),uint8_t(a^b)};
}
inline void validateSimple(const Graph& g) {
    size_t n=g.size();
    for (const auto& row:g) if (row.size()!=n) throw invalid_argument("square graph");
    for (size_t u=0;u<n;++u) {
        if (g[u][u]) throw invalid_argument("self-loop");
        for (size_t v=0;v<n;++v) if (g[u][v]!=g[v][u]) throw invalid_argument("undirected graph");
    }
}
using Cycle=array<int,4>;
inline optional<Cycle> fourCycleBrute(const Graph& g,U64* quadruples=nullptr) {
    validateSimple(g);int n=g.size();
    for (int a=0;a<n;++a) for (int b=a+1;b<n;++b) for (int c=b+1;c<n;++c) for (int d=c+1;d<n;++d) {
        if (quadruples) ++*quadruples;
        if (g[a][b]&&g[b][c]&&g[c][d]&&g[d][a]) return Cycle{a,b,c,d};
        if (g[a][b]&&g[b][d]&&g[d][c]&&g[c][a]) return Cycle{a,b,d,c};
        if (g[a][c]&&g[c][b]&&g[b][d]&&g[d][a]) return Cycle{a,c,b,d};
    }
    return nullopt;
}
inline optional<Cycle> fourCycleFast(const Graph& g,U64* pairs=nullptr) {
    validateSimple(g);int n=g.size();vector<vector<int>> common(n,vector<int>(n,-1));
    for (int v=0;v<n;++v) {
        vector<int> neighbors;
        for (int s=0;s<n;++s) if (g[v][s]) neighbors.push_back(s);
        for (size_t i=0;i<neighbors.size();++i) for (size_t j=i+1;j<neighbors.size();++j) {
            int s=neighbors[i],t=neighbors[j];
            if (pairs) ++*pairs;
            if (common[s][t]>=0) return Cycle{s,v,t,common[s][t]};
            common[s][t]=v;
        }
    }
    return nullopt;
}
inline Graph sampleFourCycle(bool square) {
    Graph g(4,vector<bool>(4));
    for (auto [u,v]:vector<pair<int,int>>{{0,1},{1,2},{2,3}}) g[u][v]=g[v][u]=true;
    if (square) g[0][3]=g[3][0]=true;
    return g;
}
inline void printCycle(Cycle c) {
    for (int v:c) cout<<v+1<<' ';
    cout<<c[0]+1<<'\n';
}
} // namespace nycu
