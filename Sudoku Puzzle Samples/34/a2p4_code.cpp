#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <vector>
#include <list>
#include <cmath>
#include <map>
#include <utility>
#include <string>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

struct cell{
    int r;                             //row
    int c;                             //column
    int value;
    int h;
    cell *parent;                      //NULL if it is the root of tree
    vector<cell*> children;            //ordered by least constraining heuristic
};


bool sortfunction(cell *a, cell *b){
    return a->h < b->h;
}


int tie_breaker(vector<vector<int> >& puzzle, int r, int c){
    int result;
    for(int i = 0; i < 9; ++i){
        if(i != r && puzzle[i][c] == 0){
            result += 1;
        }
    }
    for(int j = 0; j < 9; ++j){
        if(j != c && puzzle[r][j] == 0){
            result += 1;
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            int row = r / 3 * 3 + i;
            int column = c / 3 * 3 + j;
            if(row != r && column != c && puzzle[row][column] == 0){
                result += 1;
            }
        }
    }
    return result;
}

int values_reduced(vector<vector<int> >& puzzle, map<pair<int,int>, unordered_set<int> >& unassigned_pos, int r, int c, int value){
    int result;
    pair<int, int> p;
    for(int i = 0; i < 9; ++i){
        p.first = i;
        p.second = c;
        if(i != r && puzzle[i][c] == 0 && unassigned_pos.find(p) != unassigned_pos.end() && unassigned_pos[p].find(value) != unassigned_pos[p].end()){
            result += 1;
        }
    }
    for(int j = 0; j < 9; ++j){
        p.first = r;
        p.second = j;
        if(j != c && puzzle[r][j] == 0 && unassigned_pos.find(p) != unassigned_pos.end() && unassigned_pos[p].find(value) != unassigned_pos[p].end()){
            result += 1;
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            int row = r / 3 * 3 + i;
            int column = c / 3 * 3 + j;
            p.first = row;
            p.second = column;
            if(row != r && column != c && puzzle[row][column] == 0 && unassigned_pos.find(p) != unassigned_pos.end() && unassigned_pos[p].find(value) != unassigned_pos[p].end()){
                result += 1;
            }
        }
    }
    return result;
}

void generate_children(vector<vector<int> >& puzzle, map<pair<int,int>, unordered_set<int> >& unassigned_pos, cell *parent, vector<cell*> &children){
    int min = INT_MAX;
    int r;
    int c;
    int tie;
    for(map<pair<int,int>, unordered_set<int> >::iterator it = unassigned_pos.begin(); it != unassigned_pos.end(); ++it){
        if(puzzle[it->first.first][it->first.second] == 0){
            if(it->second.size() < min){
                min = it->second.size();
                r = it->first.first;
                c = it->first.second;
                tie = tie_breaker(puzzle, r, c);
            }
            if(it->second.size() == min){
                int cmp = tie_breaker(puzzle, it->first.first, it->first.second);
                if(cmp > min){
                    r = it->first.first;
                    c = it->first.second;
                    tie = cmp;
                }
            }
            
        }
        
    }
    //now we know the position
    //then, order values
    //sort according to their h valus
    pair<int, int> p;
    p.first = r;
    p.second = c;
    for(unordered_set<int>::iterator it = unassigned_pos[p].begin(); it != unassigned_pos[p].end(); ++it){
        cell *child = new cell();
        child->r = r;
        child->c = c;
        child->value = *it;
        child->h = values_reduced(puzzle, unassigned_pos, r, c, child->value);
        child->parent = parent;
        children.push_back(child);
    }
    sort(children.begin(), children.end(), sortfunction);
}

bool isValid(vector<vector<int> >& puzzle, int r,int c, int value){
    for(int i = 0; i < 9; ++i){
        if(puzzle[i][c] == value){
            return false;
        }
    }
    for(int j = 0; j < 9; ++j){
        if(puzzle[r][j] == value){
            return false;
        }
    }
    for(int i = (r / 3) * 3; i < (r / 3) * 3 + 2; ++i){
        for(int j = (c / 3) * 3; j < (c / 3) * 3 + 2; ++j){
            if(puzzle[i][j] == value){
                return false;
            }
        }
    }
    return true;
    
}

void unassigned_insert(vector<vector<int> >& puzzle, map<pair<int,int>, unordered_set<int> >& unassigned_pos, int r, int c, int value){
    pair<int, int> p;
    for(map<pair<int,int>, unordered_set<int> >::iterator it = unassigned_pos.begin(); it != unassigned_pos.end(); ++it){
        p.first = it->first.first;
        p.second = it->first.second;
        if(it->first.first == r && it->first.second == c){
            //do nothing
        }else{
            if(p.first == r && p.second != c && isValid(puzzle, p.first, p.second, value)){
                it->second.insert(value);
            }
            if(p.first != r && p.second == c && isValid(puzzle, p.first, p.second, value)){
                it->second.insert(value);
            }
            int row1 = (r / 3) * 3;
            int row2 = (r / 3) * 3 + 2;
            int column1 = (c / 3) * 3;
            int column2 = (c / 3) * 3 + 2;
            if(row1 <= p.first && p.first <= row2 && column1 <= p.second && p.second <= column2 && p.first != r && p.second != c && isValid(puzzle, p.first, p.second, value)){
                it->second.insert(value);
            }
        }
    }
}

void unassigned_update(map<pair<int,int>, unordered_set<int> >& unassigned_pos, int r, int c, int value){
    pair<int, int> p;
    for(map<pair<int,int>, unordered_set<int> >::iterator it = unassigned_pos.begin(); it != unassigned_pos.end(); ++it){
        p.first = it->first.first;
        p.second = it->first.second;
        if(it->first.first == r && it->first.second == c){
            //do nothing
        }else{
            if(p.first == r && p.second != c){
                it->second.erase(value);
            }
            if(p.first != r && p.second == c){
                it->second.erase(value);
            }
            int row1 = (r / 3) * 3;
            int row2 = (r / 3) * 3 + 2;
            int column1 = (c / 3) * 3;
            int column2 = (c / 3) * 3 + 2;
            if(row1 <= p.first && p.first <= row2 && column1 <= p.second && p.second <= column2 && p.first != r && p.second != c){
                it->second.erase(value);
            }
        }
    }
}

//return true if an unassigned pos has no more valid moves
//false, otherwise
bool forward_check(vector<vector<int> >& puzzle, map<pair<int,int>, unordered_set<int> >& unassigned_pos){
    for(map<pair<int,int>, unordered_set<int> >::iterator it = unassigned_pos.begin(); it != unassigned_pos.end(); ++it){
        if(puzzle[it->first.first][it->first.second] == 0 && it->second.size() == 0){
            return true;
        }
    }
    return false;
}



//return number of variable assignments
//return -1 if steps are more than 10000
int CSP(vector<vector<int> >& puzzle){
    int steps = 0;
    map<pair<int,int>, unordered_set<int> > unassigned_pos;
    unordered_set<int> OneToNine;
    for(int i = 1; i <= 9; ++i){
        OneToNine.insert(i);
    }
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            if(puzzle[i][j] == 0){
                pair<int, int> pos;
                pos.first = i;
                pos.second = j;
                unassigned_pos[pos] = OneToNine;
                for(int row = 0; row < 9; ++row){
                    if(row != i && puzzle[row][j] != 0){
                        if(unassigned_pos[pos].find(puzzle[row][j]) != unassigned_pos[pos].end()){
                            unassigned_pos[pos].erase(puzzle[row][j]);
                        }
                    }
                }
                for(int column = 0; column < 9; ++column){
                    if(column != j && puzzle[i][column] != 0){
                        if(unassigned_pos[pos].find(puzzle[i][column]) != unassigned_pos[pos].end()){
                            unassigned_pos[pos].erase(puzzle[i][column]);
                        }
                    }
                }
                for(int m = 0; m < 3; ++m){
                    for(int n = 0; n < 3; ++n){
                        int row = i / 3 * 3 + m;
                        int column = j / 3 * 3 + n;
                        if(row != i && column != j && puzzle[row][column] != 0){
                            if(unassigned_pos[pos].find(puzzle[row][column]) != unassigned_pos[pos].end()){
                                unassigned_pos[pos].erase(puzzle[row][column]);
                            }
                        }
                    }
                }
            }
        }
    }
    
    cell *root = new cell();
    root->parent = NULL;
    generate_children(puzzle, unassigned_pos, root, root->children);
    cell *current = root;
    int unassigned_total = unassigned_pos.size();

    while(unassigned_total > 0 && steps <= 10000){
        //back_tracking
        if(current->children.size() == 0){
            puzzle[current->r][current->c] = 0;
            unassigned_total += 1;
            unassigned_insert(puzzle, unassigned_pos, current->r, current->c, current->value);
            current = current->parent;
            continue;
        }else if(forward_check(puzzle, unassigned_pos)){
            puzzle[current->r][current->c] = 0;
            unassigned_total += 1;
            unassigned_insert(puzzle, unassigned_pos, current->r, current->c, current->value);
            current = current->parent;
            continue;
        }else{
            cell *prev = current;
            current = current->children[0];
            prev->children.erase(prev->children.begin());
        }
            puzzle[current->r][current->c] = current->value;
            cout << "(" <<current->r << ", " << current->c << "), " << current->value;
            unassigned_update(unassigned_pos, current->r, current->c, current->value);
            generate_children(puzzle, unassigned_pos, current, current->children);
            unassigned_total -= 1;
            steps += 1;
    }
    
    
    //either find a valid solution or steps are more than 10000
    if(unassigned_total == 0){
        cout << "puzzle solved" << endl;
        for(int i = 0; i < 9; ++i){
            for(int j = 0; j < 9; ++j){
                cout << puzzle[i][j] << " ";
            }
            cout << endl;
        }
        
        return steps;
    }else{
        //fail
        return -1;
    }
}


int main(){
    int total_assignments;
    int valid_puzzle;
    for(int i = 1; i <= 10; ++i){
        vector<vector<int> > puzzle;
        ifstream myfile;
        myfile.open(to_string(i) + ".sd");
        int cell;
        for(int m = 0; m < 9; ++m){
            vector<int> row;
            for(int n = 0; n < 9; ++n){
                myfile >>  cell;
                row.push_back(cell);
            }
            puzzle.push_back(row);
        }
        int sub_total = CSP(puzzle);
        if(sub_total != -1){
            total_assignments += sub_total;
            valid_puzzle += 1;
        }
    }
    //output average number of steps
    cout << "**************" << endl;
    cout << "Summary:" << endl;
    cout << "valid instances: " << valid_puzzle << endl;
    cout << "average number of steps is " << total_assignments / valid_puzzle << endl;
}

