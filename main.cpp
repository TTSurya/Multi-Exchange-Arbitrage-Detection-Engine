#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<fstream>
#include<sstream>
#include<cmath>
#include<algorithm>

using namespace std;

struct Edge{
    string u,v;
    double rate,w;
};

int main(){

    ifstream fin("rates.csv");

    if(!fin.is_open()){
        cout<<"Cannot open rates.csv\n";
        return 1;
    }

    string line;
    getline(fin,line);

    vector<Edge> edges;
    vector<string> nodes;
    unordered_map<string,int> id;

    while(getline(fin,line)){

        stringstream ss(line);

        string u,v,tmp;

        getline(ss,u,',');
        getline(ss,v,',');
        getline(ss,tmp,',');

        double rate=stod(tmp);

        if(!id.count(u)){
            id[u]=nodes.size();
            nodes.push_back(u);
        }

        if(!id.count(v)){
            id[v]=nodes.size();
            nodes.push_back(v);
        }

        edges.push_back({u,v,rate,-log(rate)});
    }

    int n=nodes.size();

    unordered_map<string,double> dist;
    unordered_map<string,string> par;

    for(auto &x:nodes)dist[x]=1e18;

    dist[nodes[0]]=0;

    for(int i=0;i<n-1;i++){
        bool changed=false;
        for(auto &e:edges){
            if(dist[e.u]<1e17&&dist[e.u]+e.w<dist[e.v]){
                dist[e.v]=dist[e.u]+e.w;
                par[e.v]=e.u;
                changed=true;
            }
        }

        if(!changed)break;
    }

    string cyc="";

    for(auto &e:edges){
        if(dist[e.u]<1e17 && dist[e.u]+e.w<dist[e.v]){
            cyc=e.v;
            break;
        }
    }

    ofstream fout("arbitrage_report.txt");

    if(cyc==""){
        cout<<"No arbitrage found\n";
        fout<<"No arbitrage found\n";
        return 0;
    }

    for(int i=0;i<n;i++)cyc=par[cyc];

    vector<string> path;
    string cur=cyc;

    do{
        path.push_back(cur);
        cur=par[cur];
    } while(cur!=cyc);

    path.push_back(cyc);
    reverse(path.begin(),path.end());

    double product=1.0,logsum=0.0,startCapital=1000.0,capital=1000.0;

    cout<<"Cycle:\n";
    fout<<"Cycle:\n";

    for(int i=0;i<path.size();i++){
        cout<<path[i];
        fout<<path[i];

        if(i+1<path.size()){
            cout<<" -> ";
            fout<<" -> ";
        }
    }

    cout<<"\n\n";
    fout<<"\n\n";

    fout<<"Edge Details\n\n";

    for(int i=0;i+1<path.size();i++){
        string a=path[i],b=path[i+1];

        for(auto &e:edges){
            if(e.u==a && e.v==b){
                product*=e.rate;
                logsum+=e.w;
                capital*=e.rate;

                fout<<a<<" -> "<<b<<"\n";
                fout<<"rate="<<e.rate<<"\n";
                fout<<"weight="<<e.w<<"\n\n";

                break;
            }
        }
    }

    double profitPct=(product-1.0)*100.0;

    cout<<"Cycle Product: "<<product<<"\n";
    cout<<"Log Sum: "<<logsum<<"\n";
    cout<<"Start Capital: "<<startCapital<<"\n";
    cout<<"End Capital: "<<capital<<"\n";
    cout<<"Profit %: "<<profitPct<<"\n";

    fout<<"Cycle Product: "<<product<<"\n";
    fout<<"Log Sum: "<<logsum<<"\n";
    fout<<"Start Capital: "<<startCapital<<"\n";
    fout<<"End Capital: "<<capital<<"\n";
    fout<<"Profit %: "<<profitPct<<"\n";

    return 0;
}