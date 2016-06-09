#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <iterator>
using namespace std;

const int UP = 1;
const int DOWN = 4;



vector<pair<int,int> > res;
vector<pair<int,int> > act;
double best=-100000;

double heuristique(vector<string> &board,char car)
{
    double nb_blancs=0.0;
    double nb_noirs=0.0;
    for(int c=0;c<board.size();c++)
        for(int c2=0;c2<board[c].size();c2++)
             if(board[c][c2]=='w') nb_blancs+=5.0+(7.0-c);
             else if(board[c][c2]=='W') nb_blancs+=5.0+7.0+2.0;
             else if(board[c][c2]=='B') nb_noirs+=5.0+c;
             else if(board[c][c2]=='b') nb_noirs+=5.0+7.0+2.0;
   return (car=='w'?nb_blancs:nb_noirs)/(nb_blancs+nb_noirs);
}

vector<pair<int,int> > op_possibles(int mask,vector<string> &board,int x,int y,bool prise,char enemy)
{
    vector<pair<int,int> > res;
    for(int c=0;c<=2;c++)
    {
        if((1<<c)&mask)
        {
            for(int c2=-1;c2<=1;c2+=2)
            {
                int x2=x+(c-1)*(prise?2:1);
                int y2=y+(c2)*(prise?2:1);
                if(x2<board.size()&&y2<board[0].size()&&min(x2,y2)>=0&&board[x2][y2]=='_'&&(!prise||tolower(board[(x2+x)/2][(y2+y)/2])==enemy))
                       res.push_back(make_pair(x2,y2));

            }
        }
    }
    return res;
}


double alphabeta(vector<string> &board,int profondeur, double alpha, double beta, bool player,char play,int contraintex,int contraintey)
{
    char enemy=(play=='b'?'w':'b');
    if(profondeur==9)
    {
        return heuristique(board,(player?play:enemy));
    }

    int dir=(play=='b'?DOWN:UP);
    vector<vector<vector<pair<int,int> > > > prises(board.size(),vector<vector<pair<int,int> > >(board[0].size()));
    vector<vector<vector<pair<int,int> > > > dep(board.size(),vector<vector<pair<int,int> > >(board[0].size()));
    bool soufflernestpasjouer = false;
    if(contraintex!=-1)
        soufflernestpasjouer = true;
    int possibilites=0;
    for(int c=0;c<board.size();c++)
    {
        for(int c2=0;c2<board[c].size();c2++)
        {
            if(contraintex!=-1&&(contraintex!=c||contraintey!=c2)) continue;
            char tmp=board[c][c2];
            if((play=='w'&&(tmp=='w'||tmp=='W'))||(play=='b'&&(tmp=='b'||tmp=='B')))
            {
                prises[c][c2]=op_possibles((tmp=='W'||tmp=='B'?UP|DOWN:dir),board,c,c2,true,enemy);
                possibilites+=prises[c][c2].size();
                if(prises[c][c2].size()!=0)
                    soufflernestpasjouer=true;
                if(!soufflernestpasjouer)
                    dep[c][c2]=op_possibles((tmp=='W'||tmp=='B'?UP|DOWN:dir),board,c,c2,false,enemy);
            }
        }
    }
    bool vide=true;
    if(!soufflernestpasjouer) swap(dep,prises);
    for(int c=0;c<board.size();c++)
        for(int c2=0;c2<board[0].size();c2++)
        {
            char pion=board[c][c2];
            for(int c3=0;c3<prises[c][c2].size();c3++)
            {
                vide=false;
                board[c][c2]='_';
                char copie_prise='n';
                int x2=prises[c][c2][c3].first,y2=prises[c][c2][c3].second;
                int x3=-1,y3=-1;
                if((x2+c)%2==0)
                {
                    x3=(x2+c)/2;
                    y3=(y2+c2)/2;
                    copie_prise=board[x3][y3];
                }
                if(x3!=-1)
                    board[x3][y3]='_';
                board[x2][y2]=(x2==0&&pion=='w'?'W':(x2==7&&pion=='b'?'B':pion));
                if(profondeur==0)
                {
                    if(contraintex==-1)
                        act.push_back(make_pair(c,c2));
                    act.push_back(make_pair(x2,y2));
                }
                if(player)
                {
                    double tmp;
                    if(soufflernestpasjouer)
                    {
                        tmp=alphabeta(board,profondeur,alpha,beta,player,play,x2,y2);
                    }
                    else
                    {
                        tmp=max(alpha,alphabeta(board,profondeur+1,alpha,beta,!player,enemy,-1,-1));
                    }
                    if(tmp>alpha)
                    {
                            if(profondeur==0&&tmp>best)
                            {
                                best=tmp;
                                res.clear();
                                res.resize(act.size());
                                copy(act.begin(),act.end(),res.begin());
                            }
                            alpha=tmp;
                    }
                    board[c][c2]=pion;
                    board[x2][y2]='_';
                    if(x3!=-1) board[x3][y3]=copie_prise;
                    if(beta<=alpha) goto end;
                }
                else
                {
                    if(soufflernestpasjouer)
                    {
                        double tmp=alphabeta(board,profondeur,alpha,beta,player,play,x2,y2);
                        if(tmp<beta)
                        {
                            beta=tmp;
                        }
                    }
                    else
                    {
                        beta=min(beta,alphabeta(board,profondeur+1,alpha,beta,!player,enemy,-1,-1));
                    }
                    board[c][c2]=pion;
                    board[prises[c][c2][c3].first][prises[c][c2][c3].second]='_';
                    if(x3!=-1) board[x3][y3]=copie_prise;
                    if(beta<=alpha) goto end;
                }
                if(profondeur==0)
                {
                    if(contraintex==-1)
                        act.pop_back();
                    act.pop_back();
                }
            }
        }
    if(vide)
    {
        return alphabeta(board,profondeur+1,alpha,beta,!player,enemy,-1,-1);
    }
    end:
        if(player) return alpha;
        else return beta;
}
int main() {
    char play;
    cin>>play;
    int taille;
    cin>>taille;
    vector<string> board(taille);
    for(int c=0;c<taille;c++) cin>>board[c];
    alphabeta(board,0,-10000,10000,true,play,-1,-1);
    cout<<res.size()-1<<endl;
    for(int c=0;c<res.size();c++)
        cout<<res[c].first<<" "<<res[c].second<<endl;

    return 0;
}
