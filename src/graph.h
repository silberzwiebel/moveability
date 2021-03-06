#pragma once

#include <Rcpp.h>
#include <algorithm> // std::find
#include <vector>
#include <map>
#include <limits>
#include <random>
#include <string> // stoi
#include <cmath> // round
#include <math.h> // isnan

const float INFINITE_FLOAT =  std::numeric_limits<float>::max ();
const double INFINITE_DOUBLE =  std::numeric_limits<double>::max ();
const int INFINITE_INT =  std::numeric_limits<int>::max ();

typedef std::string vertex_id_t, edge_id_t;
typedef std::unordered_map <unsigned int,
    std::unordered_set <unsigned int> > int2ints_map_t;

struct edge_component
{
    // used only for edge sampling on graphs without component numbers
    edge_id_t edge;
    unsigned int component;
};

struct vertex_t
{
    private:
        std::unordered_set <vertex_id_t> in, out;

    public:
        void add_neighbour_in (vertex_id_t vert_id) { in.insert (vert_id); }
        void add_neighbour_out (vertex_id_t vert_id) { out.insert (vert_id); }
        unsigned long int get_degree_in () { return in.size (); }
        unsigned long int get_degree_out () { return out.size (); }

        std::unordered_set <vertex_id_t> get_all_neighbours ()
        {
            std::unordered_set <vertex_id_t> all_neighbours = in;
            all_neighbours.insert (out.begin (), out.end ());
            return all_neighbours;
        }

        std::unordered_set <vertex_id_t> get_in_neighbours ()
        {
            return in;
        }

        std::unordered_set <vertex_id_t> get_out_neighbours ()
        {
            return out;
        }

        void replace_neighbour (vertex_id_t n_old, vertex_id_t n_new)
        {
            if (in.find (n_old) != in.end ())
            {
                in.erase (n_old);
                in.insert (n_new);
            }
            if (out.find (n_old) != out.end ())
            {
                out.erase (n_old);
                out.insert (n_new);
            }
        }

        // in can equal out, so get_all_neighbours is vital here:
        bool is_intermediate_single ()
        {
            return (in.size () == 1 && out.size () == 1 &&
                    get_all_neighbours ().size () == 2);
        }

        bool is_intermediate_double ()
        {
            return (in.size () == 2 && out.size () == 2 &&
                    get_all_neighbours ().size () == 2);
        }
};

struct edge_t
{
    private:
        vertex_id_t from, to;
        edge_id_t id;
        std::set <edge_id_t> old_edges;

    public:
        double dist;
        double weight;
        bool replaced_by_compact;

        vertex_id_t get_from_vertex () { return from; }
        vertex_id_t get_to_vertex () { return to; }
        edge_id_t getID () { return id; }
        std::set <edge_id_t> get_old_edges () { return old_edges; }

        edge_t (vertex_id_t from_id, vertex_id_t to_id,
                double dist_in, double weight_in, edge_id_t id_in,
                std::set <edge_id_t> replacement_edges)
        {
            replaced_by_compact = false;
            this -> to = to_id;
            this -> from = from_id;
            this -> dist = dist_in;
            this -> weight = weight_in;
            this -> id = id_in;
            this -> old_edges.insert (replacement_edges.begin (),
                    replacement_edges.end ());
        }
};


typedef std::unordered_map <vertex_id_t, vertex_t> vertex_map_t;
typedef std::unordered_map <edge_id_t, edge_t> edge_map_t;
typedef std::unordered_map <vertex_id_t,
        std::unordered_set <edge_id_t> > vert2edge_map_t;

//----------------------------
//----- functions in graph.cpp
//----------------------------

namespace graph {

void add_to_v2e_map (vert2edge_map_t &vert2edge_map, const vertex_id_t vid,
        const edge_id_t eid);

void erase_from_v2e_map (vert2edge_map_t &vert2edge_map, const vertex_id_t vid,
        const edge_id_t eid);

bool graph_has_components (const Rcpp::DataFrame &graph);

void graph_from_df (const Rcpp::DataFrame &gr, vertex_map_t &vm,
        edge_map_t &edge_map, vert2edge_map_t &vert2edge_map);

unsigned int identify_graph_components (vertex_map_t &v,
        std::unordered_map <vertex_id_t, unsigned int> &com);

} // end namespace graph
