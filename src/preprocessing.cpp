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

namespace banana {
namespace solver {

bool Preprocessing::lmr_reduction(Oracle::SubProblem &instance) {

  const auto& m_oracle = Environment::oracle();

  int m = 0, n = instance.size();

  auto intervals = m_oracle.getCompressedIntervals(instance);
      
  for (auto [l, r] : intervals) m = std::max({m, l, r});

  std::vector<std::vector<int>> segtree(2, std::vector<int>(4*m+5));
  // segtree[0] guarda os inicios de intervalos, segtree[1] guarda os finais de intervalos

  std::function<void(int, int, int, int, int, int)> upd = [&] (int l, int r, int p, int x, int layer, int k) {
      if (l == r) {
            segtree[layer][p] += k;
            return;
      }
      int md = (l + r) / 2;
      if (x <= md) upd(l, md, 2*p, x, layer, k);
      else upd(md+1, r, 2*p+1, x, layer, k);
      segtree[layer][p] = segtree[layer][2*p] + segtree[layer][2*p+1];
  };

  std::function<int(int, int, int, int, int)> get_left = [&] (int l, int r, int p, int a, int b) {
	if (b < l or r < a or segtree[0][p] == 0) return -1;
	if (r == l) return l;
	int m = (l+r)/2;
	int x = get_left(l, m, 2*p, a, b);
	if (x != -1) return x;
	return get_left(m+1, r, 2*p+1, a, b);
  };

  std::function<int(int, int, int, int, int)> get_right = [&] (int l, int r, int p, int a, int b) {
	if (b < l or r < a or segtree[1][p] == 0) return -1;
	if (r == l) return l;
	int m = (l+r)/2;
	int x = get_right(m+1, r, 2*p+1, a, b);
	if (x != -1) return x;
	return get_right(l, m, 2*p, a, b);
  };

  for (auto [l, r] : intervals) {
      upd(0, m-1, 1, l, 0, 1);
      upd(0, m-1, 1, r, 1, 1);
  }

  auto orient = m_oracle.getOrientablePairs(instance);

  std::vector<std::vector<int>> orient_adj(n);

  for (auto [u, v] : orient) orient_adj[u].push_back(v), orient_adj[v].push_back(u);

  // TODO: usar um gerador aleatorio alternativo (mt19937)
  for (int i = 0; i < n; ++i) random_shuffle(begin(orient_adj[i]), end(orient_adj[i]));

  std::vector<Oracle::Vertex> removidos;

  std::vector<bool> vivos(n, 1);

  for (int i = 0; i < n; ++i) {
      if (vivos[i] == 0) continue;
      Oracle::Vertex u = instance[i];
      upd(0, m-1, 1, intervals[i].first, 0, -1);
      upd(0, m-1, 1, intervals[i].second, 1, -1);
      int l_max = get_right(0, m-1, 1, 0, intervals[i].first), r_min = get_left(0, m-1, 1, intervals[i].second, m-1);
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
      if (deu_certo) {
            // tiramos o vertice u da instancia
            removidos.push_back(u);
            vivos[i] = 0;
      } else {
            upd(0, m-1, 1, intervals[i].first, 0, 1);
            upd(0, m-1, 1, intervals[i].second, 1, 1);
      }
  }     


  if (removidos.empty()) return false;

  Oracle::SubProblem new_instance;

  for (int i = 0; i < n; ++i) {
      if (vivos[i]) new_instance.push_back(instance[i]);
  }

  swap(new_instance, instance); 

  // TODO: checar se o compilador ta desaparecendo com memoria desnecessaria

  new_instance.clear();
  
// TODO: how to call base solver from here?
  BaseSolver::recursiveSolver(instance);

  for (int i = (int) removidos.size() - 1; i >= 0; --i) {
      int l_max = -1;
      for (int j = 0; j < instance.size(); ++j) {
            int uv = m_oracle.getCrossings(removidos[i], instance[j]), vu = m_oracle.getCrossings(instance[j], removidos[i]);
            if (vu < uv) l_max = j;
      } 
      instance.insert(begin(instance)+l_max+1, removidos[i]);
  }

  return true;
}

bool Preprocessing::twins(Oracle::SubProblem &instance) {
      const auto& m_oracle = Environment::oracle();

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

      Oracle::SubProblem new_instance;

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
            if (j != i + 1) twins_repr.push_back({instance[i], cur_twins});
            i = j;
      }

      if (twins_repr.empty()) return false;

      auto cmp = [&] (std::pair<Oracle::Vertex, Oracle::SubProblem> a, std::pair<Oracle::Vertex, Oracle::SubProblem> b) {
            return a.first.first < b.first.first;
      };

      sort(begin(twins_repr), end(twins_repr), cmp);

      swap(instance, new_instance); new_instance.clear();
      
      BaseSolver::recursiveSolver(instance);

      for (int i = 0; i < instance.size(); ++i) {
            int id = lower_bound(begin(twins_repr), end(twins_repr), std::make_pair(instance[i], std::vector<int>()), cmp) - begin(twins_repr);
            if (id != twins_repr.size() && twins_repr[id].first == instance[i]) {
                  for (int j = 0; j < twins_repr[id].second.size(); ++j) {
                        new_instance.push_back(twins_repr[id].second[j]);
                  }
            } else new_instance.push_back(instance[i]);
      }

      swap(instance, new_instance); new_instance.clear();
      
      return true;
}

bool Preprocessing::cut_by_pieces(Oracle::SubProblem &instance) {

      const auto& m_oracle = Environment::oracle();

      auto intervals = m_oracle.getCompressedIntervals(instance);

      enum State { FINISH, START };
      std::vector<std::tuple<int, State, int>> events; // coordinate, (termino/inicio), vertice
      for (int i = 0; i < instance.size(); ++i) {
            auto [l, r] = intervals[i];
            events.emplace_back(l, START, instance[i].first);
            events.emplace_back(r, FINISH, instance[i].first);
      }
      std::sort(begin(events), end(events));
      std::vector<Oracle::SubProblem> pieces;
      int atv = 0;
      for (auto [coord, type, vertex] : events) {
            if (type == FINISH) {
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

      return true;
}     

} // namespace solver
} // namespace banana