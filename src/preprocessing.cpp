/******************************************************************************
 * Top contributors (to current version):
 *   Gabriel Lucas, Gabriel Ubiratan, Kaio Vieira
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 */
#include "preprocessing.h"
#include "oracle.h"

#include <iostream>

namespace banana {
namespace solver {

bool Preprocessing::lmr_reduction(Oracle::SubProblem &instance) {
  const auto& m_oracle = Environment::oracle();

  int m = 0, n = instance.size(), tam_ini = instance.size();

  auto instance2 = instance;
  auto intervals = m_oracle.getCompressedIntervals(instance);
      
  for (auto [l, r] : intervals) m = std::max({m, l, r});

  std::vector<std::vector<int>> segtree(2, std::vector<int>(4*m+5));
  // segtree[0] guarda os inicios de intervalos, segtree[1] guarda os finais de intervalos

  auto upd = [&] (auto&& self, int l, int r, int p, int x, int layer, int k) {
      if (l == r) {
            segtree[layer][p] += k;
            return;
      }
      int md = (l + r) / 2;
      if (x <= md) self(self, l, md, 2*p, x, layer, k);
      else self(self, md+1, r, 2*p+1, x, layer, k);
      segtree[layer][p] = segtree[layer][2*p] + segtree[layer][2*p+1];
  };

  auto get_left = [&] (auto&& self, int l, int r, int p, int a, int b) -> int {
	if (b < l or r < a or segtree[0][p] == 0) return m+1;
	if (r == l) return l;
	int md = (l+r)/2;
	int x = self(self, l, md, 2*p, a, b);
	if (x != m+1) return x;
	return self(self, md+1, r, 2*p+1, a, b);
  };

   auto get_right = [&] (auto&& self, int l, int r, int p, int a, int b) -> int {
	if (b < l or r < a or segtree[1][p] == 0) return -1;
	if (r == l) return l;
	int m = (l+r)/2;
	int x = self(self, m+1, r, 2*p+1, a, b);
	if (x != -1) return x;
	return self(self, l, m, 2*p, a, b);
  };

  for (auto [l, r] : intervals) {
      upd(upd, 0, m, 1, l, 0, 1);
      upd(upd, 0, m, 1, r, 1, 1);
  }

  auto orient = m_oracle.getOrientablePairs(instance);

  std::vector<std::vector<int>> orient_adj(n);

  for (auto [u, v] : orient) orient_adj[u].push_back(v), orient_adj[v].push_back(u);

  // TODO: usar um gerador aleatorio alternativo (mt19937)
  for (int i = 0; i < n; ++i) random_shuffle(begin(orient_adj[i]), end(orient_adj[i]));

  std::vector<Oracle::Vertex> removidos;

  std::vector<int> vivos(n, 1);

  for (int i = 0; i < n; ++i) {
      Oracle::Vertex u = instance[i];
      upd(upd, 0, m, 1, intervals[i].first, 0, -1);
      upd(upd, 0, m, 1, intervals[i].second, 1, -1);
      int l_max = get_right(get_right, 0, m, 1, 0, intervals[i].first);
      int r_min = get_left(get_left, 0, m, 1, intervals[i].second, m);
      bool deu_certo = 1;
      for (auto vi : orient_adj[i]) {
            if (vivos[vi] == 0) continue;
            Oracle::Vertex v = instance[vi];
            int uv = m_oracle.getCrossings(u, v), vu = m_oracle.getCrossings(v, u);
            if (uv < vu) {
                  // quero ficar na direita
                  r_min = std::min(r_min, intervals[vi].first);
            } else if (vu < uv) {
                  // quero ficar na esquerda
                  l_max = std::max(l_max, intervals[vi].second);
            }
            if (r_min < l_max) {
                  deu_certo = 0;
                  break;
            }
      }
      // int l_max2 = -1, r_min2 = m + 1;
      // for (int j = 0; j < n; ++j) {
      //       if (i == j || vivos[j] == 0) continue;
      //       Oracle::Vertex v = instance[j];
      //       int uv = m_oracle.getCrossings(u, v), vu = m_oracle.getCrossings(v, u);
      //       if (uv < vu) r_min2 = std::min(r_min2, intervals[j].first); 
      //       if (vu < uv) l_max2 = std::max(l_max2, intervals[j].second);
      // }
      // assert(r_min2 == r_min && l_max2 == l_max);
      if (deu_certo) {
            // tiramos o vertice u da instancia
            removidos.push_back(u);
            vivos[i] = 0;
      } else {
            upd(upd, 0, m, 1, intervals[i].first, 0, 1);
            upd(upd, 0, m, 1, intervals[i].second, 1, 1);
      }
      
  }     

  if (removidos.empty()) return false;

  Oracle::SubProblem new_instance;

  for (int i = 0; i < n; ++i) {
      if (vivos[i]) new_instance.push_back(instance[i]);
  }

  swap(new_instance, instance); 

  new_instance.clear();
  // TODO: checar se o compilador ta desaparecendo com memoria desnecessaria
  
  BaseSolver::recursiveSolver(instance);

      for (int i = (int) removidos.size() - 1; i >= 0; --i) {
            int l_max = -1;
            for (int j = 0; j < instance.size(); ++j) {
                  int uv = m_oracle.getCrossings(removidos[i], instance[j]), vu = m_oracle.getCrossings(instance[j], removidos[i]);
                  if (vu < uv) l_max = j;
            } 
            if (instance.empty()) instance.push_back(removidos[i]);
            else instance.insert(begin(instance)+l_max+1, removidos[i]);
      }

      int tam_fim = instance.size();
      assert(tam_fim == tam_ini);

  return true;
}


void Preprocessing::twins(Oracle::SubProblem &instance) {
      const auto& m_oracle = Environment::oracle();

      int tam_ini = instance.size();

      std::sort(begin(instance), end(instance), [&] (Oracle::Vertex a, Oracle::Vertex b) {
            auto va = m_oracle.neighborhood(a.first);
            auto vb = m_oracle.neighborhood(b.first);
            int n = std::min(va.size(), vb.size());
            for (int i = 0; i < n; ++i) {
                  if (va[i] != vb[i]) return va[i] < vb[i];
            }
            return va.size() < vb.size();
      });  
      int i = 0, n = instance.size();
      for (auto a : instance) {
            auto va = m_oracle.neighborhood(a.first);
            for (int i = 1; i < va.size(); ++i) assert(va[i] > va[i-1]);
      }
      Oracle::SubProblem new_instance;
      // twins_repr guarda um representante dos gemeos (com a fracao dele sendo a fracao da soma) e todos os gemeos 
      std::vector<std::pair<Oracle::Vertex, Oracle::SubProblem>> twins_repr;

      while (i < n) {
            new_instance.push_back(instance[i]);
            int j = i+1;
            auto vi = m_oracle.neighborhood(instance[i].first);
            Oracle::SubProblem cur_twins = {instance[i]};
            for (; j < n && m_oracle.neighborhood(instance[j].first) == vi; ++j) {
                  new_instance.back().second = new_instance.back().second + instance[j].second;     
                  cur_twins.push_back(instance[j]);
            }     
            if (j != i + 1) twins_repr.push_back({new_instance.back(), cur_twins});
            i = j;
      }


      sort(begin(twins_repr), end(twins_repr));

      instance = move(new_instance);

      BaseSolver::recursiveSolver(instance);

      for (int i = 1; i < twins_repr.size(); ++i) {
            assert(twins_repr[i-1].first < twins_repr[i].first);
      }

      for (int i = 0; i < (int) instance.size(); ++i) {
            int id = lower_bound(begin(twins_repr), end(twins_repr), std::make_pair(instance[i], Oracle::SubProblem())) - begin(twins_repr);
            if (id != twins_repr.size() && twins_repr[id].first == instance[i]) {
                  for (int j = 0; j < twins_repr[id].second.size(); ++j) {
                        new_instance.push_back(twins_repr[id].second[j]);
                  }
            } else new_instance.push_back(instance[i]);
      }


      instance = move(new_instance);

      int tam_fim = instance.size();

      assert(tam_fim == tam_ini);
}

bool Preprocessing::lmr_reduction2(Oracle::SubProblem &instance) {
  const auto& m_oracle = Environment::oracle();

  int m = 0, n = instance.size(), tam_ini = instance.size();

  auto instance2 = instance;
  auto intervals = m_oracle.getCompressedIntervals(instance);
      
  for (auto [l, r] : intervals) m = std::max({m, l, r});

  std::vector<std::vector<int>> segtree(2, std::vector<int>(4*m+5));
  // segtree[0] guarda os inicios de intervalos, segtree[1] guarda os finais de intervalos

  auto upd = [&] (auto&& self, int l, int r, int p, int x, int layer, int k) {
      if (l == r) {
            segtree[layer][p] += k;
            return;
      }
      int md = (l + r) / 2;
      if (x <= md) self(self, l, md, 2*p, x, layer, k);
      else self(self, md+1, r, 2*p+1, x, layer, k);
      segtree[layer][p] = segtree[layer][2*p] + segtree[layer][2*p+1];
  };

  auto get_left = [&] (auto&& self, int l, int r, int p, int a, int b) -> int {
	if (b < l or r < a or segtree[0][p] == 0) return m+1;
	if (r == l) return l;
	int md = (l+r)/2;
	int x = self(self, l, md, 2*p, a, b);
	if (x != m+1) return x;
	return self(self, md+1, r, 2*p+1, a, b);
  };

   auto get_right = [&] (auto&& self, int l, int r, int p, int a, int b) -> int {
	if (b < l or r < a or segtree[1][p] == 0) return -1;
	if (r == l) return l;
	int m = (l+r)/2;
	int x = self(self, m+1, r, 2*p+1, a, b);
	if (x != -1) return x;
	return self(self, l, m, 2*p, a, b);
  };

  for (auto [l, r] : intervals) {
      upd(upd, 0, m, 1, l, 0, 1);
      upd(upd, 0, m, 1, r, 1, 1);
  }

  auto orient = m_oracle.getOrientablePairs(instance);

  std::vector<std::vector<int>> orient_adj(n);

  for (auto [u, v] : orient) orient_adj[u].push_back(v), orient_adj[v].push_back(u);

  // TODO: usar um gerador aleatorio alternativo (mt19937)
  for (int i = 0; i < n; ++i) random_shuffle(begin(orient_adj[i]), end(orient_adj[i]));

  std::vector<Oracle::SubProblem> removidos;

  std::vector<int> vivos(n, 1);
  std::vector<std::vector<int>> candidates(n);
  std::vector<int> preference(n, 0);
  for (int i = 0; i < n; ++i) candidates[i].push_back(i);
  for (int w = 0; w < 5; ++w) {
      std::vector<std::vector<int>> new_candidates;
      for (auto& x : candidates) std::sort(begin(x), end(x));
      std::sort(begin(candidates), end(candidates));
      candidates.erase(unique(begin(candidates), end(candidates)), end(candidates));
      for (auto candidate : candidates) {
            auto [my_l, my_r] = intervals[candidate[0]];
            bool todos_vivos = 1;
            for (auto i : candidate) {
                  my_l = std::min(my_l, intervals[i].first);
                  my_r = std::max(my_r, intervals[i].second);
                  todos_vivos &= vivos[i];
            }
            if (todos_vivos == 0) continue;
            std::vector<int> vizinhos;
            for (auto i : candidate) {
                  upd(upd, 0, m, 1, intervals[i].first, 0, -1);
                  upd(upd, 0, m, 1, intervals[i].second, 1, -1);
                  vivos[i] = 0;
                  for (auto vi : orient_adj[i]) if (vivos[vi] == 1) vizinhos.push_back(vi);
            }
            std::sort(begin(vizinhos), end(vizinhos));
            vizinhos.erase(unique(begin(vizinhos), end(vizinhos)), end(vizinhos));
            bool deu_certo = 1, cancela = 0;
            int l_max = get_right(get_right, 0, m, 1, 0, my_l), r_min = get_left(get_left, 0, m, 1, my_r, m);
            int mais_esq = -1, mais_dir = -1;
            for (auto vi : vizinhos) {
                  Oracle::Vertex v = instance[vi];
                  int tipo = 0;
                  for (auto i : candidate) {
                        Oracle::Vertex u = instance[i];
                        int uv = m_oracle.getCrossings(u, v), vu = m_oracle.getCrossings(v, u);
                        if (uv < vu) {
                              // quero ficar na direita
                              tipo |= 1;
                              int x = intervals[vi].first;
                              if (r_min > x) r_min = x, mais_dir = vi;
                        } else if (vu < uv) {
                              // quero ficar na esquerda
                              tipo |= 2;
                              int x = intervals[vi].second;
                              if (l_max < x) l_max = x, mais_esq = vi;
                        }
                        if (tipo == 3) {
                              deu_certo = 0, cancela = 1;
                              break;
                        }
                  }
                  if (cancela) break;
            }
            deu_certo = (l_max <= r_min);
            if (!cancela && deu_certo) {
                  // std::cout<<"LMR " << w+1 << std::endl;
                  Oracle::SubProblem r_problem;
                  for (auto u : candidate) r_problem.push_back(instance[u]);
                  removidos.push_back(r_problem);
            } else {
                  for (auto u : candidate) {
                        upd(upd, 0, m, 1, intervals[u].first, 0, 1);
                        upd(upd, 0, m, 1, intervals[u].second, 1, 1);
                        vivos[u] = 1;
                  }
                  if (!cancela) {
                        if (mais_esq != -1) {
                              candidate.push_back(mais_esq);
                              new_candidates.push_back(candidate);
                              candidate.pop_back();
                        }
                        if (mais_dir != -1) {
                              candidate.push_back(mais_dir);
                              new_candidates.push_back(candidate);
                              candidate.pop_back();
                        }
                  }
            }
      }     
      swap(candidates, new_candidates);
  }     

  if (removidos.empty()) return false;

  Oracle::SubProblem new_instance;

  for (int i = 0; i < n; ++i) {
      if (vivos[i]) new_instance.push_back(instance[i]);
  }

  swap(new_instance, instance); 

  new_instance.clear();
  // TODO: checar se o compilador ta desaparecendo com memoria desnecessaria
  
  BaseSolver::recursiveSolver(instance); 

      for (int i = (int) removidos.size() - 1; i >= 0; --i) {
            BaseSolver::recursiveSolver(removidos[i]);
            int l_max = -1;
            for (int j = 0; j < instance.size(); ++j) {
                  for (auto k : removidos[i]) {
                        int uv = m_oracle.getCrossings(k, instance[j]), vu = m_oracle.getCrossings(instance[j], k);
                        if (vu < uv) l_max = j;
                  }
            } 
            if (instance.empty()) {
                  for (auto u : removidos[i]) instance.push_back(u);
            }
            else {
                  for (int j = (int) removidos[i].size() - 1; j >= 0; --j) {
                        instance.insert(begin(instance)+l_max+1, removidos[i][j]);
                  }
            }
      }
      // std::cout<<"TANKEI TUDO!!!"<<std::endl;
      int tam_fim = instance.size();
      assert(tam_fim == tam_ini);

  return true;
}

bool Preprocessing::cut_by_pieces(Oracle::SubProblem &instance) {

      const auto& m_oracle = Environment::oracle();
      
      auto intervals = m_oracle.getIntervals(instance);

      int tam_ini = instance.size();

      enum State { LEAF, FINISH, START };
      std::vector<std::tuple<int, State, int>> events; // coordinate, (termino/inicio), vertice
      for (int i = 0; i < instance.size(); ++i) {
            auto [l, r] = intervals[i];
            if (l == r) {
                  events.emplace_back(l, LEAF, i);
            } else {    
                  events.emplace_back(l, START, i);
                  events.emplace_back(r, FINISH, i);
            }
      }
      std::sort(begin(events), end(events));
      std::vector<Oracle::SubProblem> pieces;
      int atv = 0;
      for (auto [coord, type, vertex] : events) {
            if (type == LEAF) {
                  if (atv == 0) pieces.push_back({instance[vertex]});
                  else pieces.back().push_back(instance[vertex]); 
            } else if (type == FINISH) {
                  pieces.back().push_back(instance[vertex]);
                  atv--;
            } else {
                  if (atv == 0) pieces.push_back({});
                  atv++;
            }
      }

      if (pieces.size() == 1) return false;
      instance.clear();
      for (auto piece : pieces) {
            BaseSolver::recursiveSolver(piece);
            for (auto u : piece) instance.push_back(u);
            piece.clear();
      }

      int tam_fim = instance.size();

      assert(tam_fim == tam_ini);
      return true;
}

void Preprocessing::kill_isolated(Oracle::SubProblem &instance) {
      Oracle::SubProblem new_instance;
      Oracle::SubProblem isolated;
      auto& m_oracle = Environment::oracle();
      for (auto vertex : instance) {
            if (m_oracle.neighborhood(vertex.first).empty()) isolated.push_back(vertex);
            else new_instance.push_back(vertex);
      }
      instance = move(new_instance);
      // BaseSolver::recursiveSolver(instance);
      Preprocessing::twins(instance);
      for (auto vertex : isolated) instance.push_back(vertex);
}

bool Preprocessing::generalized_twins(Oracle::SubProblem &instance) {
      auto& m_oracle = Environment::oracle();
      int n = instance.size();
      std::vector<std::vector<int>> adj_a, adj_b(n);
      std::vector<int> c;
      for (int i = 0; i < n; ++i) {
            auto adj = m_oracle.neighborhood(instance[i].first);
            for (int j = 0; j < adj.size(); j += instance[i].second.den())  {
                  auto v = adj[j];
                  c.push_back(v), adj_b[i].push_back(v);
            }
      }
      int m = c.size();
      std::sort(begin(c), end(c));
      c.erase(unique(begin(c), end(c)), end(c));
      adj_a.resize(m);
      for (int i = 0; i < n; ++i) {
            for (auto& v : adj_b[i]) {
                  v = lower_bound(begin(c), end(c), v) - begin(c);
                  adj_a[v].push_back(i);
            }
      }
      std::vector<int> prdg(m+1);
      std::set<int> s;
      for (int i = 1; i <= m; ++i) prdg[i] = prdg[i-1] + adj_a[i-1].size();
      for (int i = 0; i < m; ++i) {
            std::vector<std::pair<int, int>> inter;
            int num_inter = 0, sum_deg = 0, ok = 0, fail = 0;
            for (int j = i; j < m; ++j) {
                  for (auto v : adj_a[j]) {
                        if (s.find(v) != end(s)) continue;
                        s.insert(v);
                        sum_deg += adj_b[v].size();
                        int g = std::gcd(num_inter, (int)adj_b[v].size());
                        if (g != num_inter) {
                              if (num_inter == 0) {
                                    inter = std::vector<std::pair<int, int>>(g, {m+1, -1});
                              } else {
                                    // mergear intervalos
                                    int k = num_inter / g;
                                    std::vector<std::pair<int, int>> new_inter(g, {m+1, -1});
                                    for (int w = 0; w < num_inter; w++) {// new_inter[j/k]
                                          new_inter[w/k].first = std::min(new_inter[w/k].first, inter[w].first);
                                          new_inter[w/k].second = std::max(new_inter[w/k].second, inter[w].second);
                                    }
                                    swap(inter, new_inter);
                                    new_inter.clear();
                              }
                              num_inter = g;
                        } 
                        int k = adj_b[v].size() / num_inter;
                        for (int w = 0; w < adj_b[v].size(); ++w) {
                              int x = w/k;
                              inter[x].first = std::min(inter[x].first, adj_b[v][w]);
                              inter[x].second = std::max(inter[x].second, adj_b[v][w]);
                              if (inter[x].first != inter[x].second && inter[x].first < i) {
                                    fail = 1; break;
                              }
                        }
                        if (fail) break;
                        
                  }
                  if (fail) break;
                  if (s.size() == 1) continue;
                  int cur_l = -1, cur_r = -1, qnt = 0;
                  ok = 1;
                  inter.push_back({m + 1, m + 1});
                  for (auto [l, r] : inter) {
                        assert(cur_r <= r && cur_l <= l);
                        if (cur_l == -1) {
                              qnt = 1, cur_l = l, cur_r = r;
                              continue;
                        }
                        if (l > cur_r) {
                              int tot = prdg[cur_r+1] - prdg[cur_l];
                              if (cur_l != cur_r && tot != sum_deg / num_inter * qnt) {
                                    ok = 0;
                                    break;
                              }
                              cur_l = l, cur_r = r, qnt = 1;
                        } else {
                              cur_r = r;
                              qnt++;
                        }
                  }
                  if (ok) break;     
                  inter.pop_back();
            }
            if (fail) {
                  s.clear();
                  continue;
            }
            if (ok && s.size() != instance.size()) break;
            s.clear();
      }

      if (s.empty()) return false;

      Oracle::SubProblem s_instance, new_instance;

      Oracle::Vertex repr = instance[(*begin(s))];

      int s_deg = 0, best_deg = m_oracle.degree(repr.first);

      for (int i = 0; i < n; ++i) {
            if (s.find(i) == end(s)) { new_instance.push_back(instance[i]); continue; }
            s_instance.push_back(instance[i]);
            int cur_deg = m_oracle.degree(instance[i].first);
            Oracle::F frac = instance[i].second * (Oracle::F) cur_deg;
            assert(frac.den() == 1);
            if (cur_deg < best_deg) best_deg = cur_deg, repr = instance[i];
            s_deg += cur_deg;
      }

      repr.second = Oracle::F(s_deg, best_deg);
      new_instance.push_back(repr);

      swap(new_instance, instance);
      new_instance.clear();

      // std::cout << "Conjunto S: ";

      // for (auto x : s_instance) std::cout << x.first + 11 << ' '; std::cout<<std::endl;

      // std::cout << "Representante: " << repr.first + 11 << std::endl; 
      
      // std::cout << "resto da instancia: ";
      // for (auto x : instance) std::cout << x.first + 11 << ' '; std::cout<<std::endl;

      BaseSolver::recursiveSolver(s_instance);
      BaseSolver::recursiveSolver(instance);
      
      for (auto u : instance) {
            if (u == repr) {
                  for (auto v : s_instance) new_instance.push_back(v);
            } else new_instance.push_back(u);
      }

      swap(new_instance, instance);

      new_instance.clear();
      // std::cout << "reduzi instancias" << std::endl;
      return true;
}

} // namespace solver
} // namespace banana