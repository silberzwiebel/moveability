#include "graph.h"

void graph::add_to_v2e_map (vert2edge_map_t &vert2edge_map, const vertex_id_t vid,
        const edge_id_t eid)
{
    std::unordered_set <edge_id_t> edge_ids;
    if (vert2edge_map.find (vid) == vert2edge_map.end ())
    {
        edge_ids.emplace (eid);
        vert2edge_map.emplace (vid, edge_ids);
    } else
    {
        edge_ids = vert2edge_map [vid];
        edge_ids.insert (eid);
        vert2edge_map [vid] = edge_ids;
    }
}

void graph::erase_from_v2e_map (vert2edge_map_t &vert2edge_map,
        const vertex_id_t vid, const edge_id_t eid)
{
    std::unordered_set <edge_id_t> edge_ids = vert2edge_map [vid];
    if (edge_ids.find (eid) != edge_ids.end ())
    {
        edge_ids.erase (eid);
        vert2edge_map [vid] = edge_ids;
    }
}

//' graph_has_components
//'
//' Does a graph have a vector of connected component IDs? Only used in
//' \code{sample_one_vertex}
//' @noRd
bool graph::graph_has_components (const Rcpp::DataFrame &graph)
{
    Rcpp::CharacterVector graph_names = graph.attr ("names");
    for (auto n: graph_names)
        if (n == "component")
            return true;

    return false;
}


//' graph_from_df
//'
//' Convert a standard graph data.frame into an object of class graph. Graphs '
//are standardised with the function \code{dodgr_convert_graph()$graph}, and
//contain ' only the four columns [from, to, d, w]
//'
//' @noRd
void graph::graph_from_df (const Rcpp::DataFrame &gr, vertex_map_t &vm,
        edge_map_t &edge_map, vert2edge_map_t &vert2edge_map)
{
    Rcpp::StringVector edge_id = gr ["edge_id"];
    Rcpp::StringVector from = gr ["from"];
    Rcpp::StringVector to = gr ["to"];
    Rcpp::NumericVector dist = gr ["d"];
    Rcpp::NumericVector weight = gr ["w"];

    std::set <edge_id_t> replacement_edges; // all empty here

    for (int i = 0; i < to.length (); i ++)
    {
        Rcpp::checkUserInterrupt ();
        vertex_id_t from_id = std::string (from [i]);
        vertex_id_t to_id = std::string (to [i]);

        if (vm.find (from_id) == vm.end ())
        {
            vertex_t fromV = vertex_t ();
            vm.emplace (from_id, fromV);
        }
        vertex_t from_vtx = vm.at (from_id);
        from_vtx.add_neighbour_out (to_id);
        vm [from_id] = from_vtx;

        if (vm.find (to_id) == vm.end ())
        {
            vertex_t toV = vertex_t ();
            vm.emplace (to_id, toV);
        }
        vertex_t to_vtx = vm.at (to_id);
        to_vtx.add_neighbour_in (from_id);
        vm [to_id] = to_vtx;

        edge_id_t edge_id_str = Rcpp::as <edge_id_t> (edge_id [i]);

        double wt = weight [i];
        if (weight [i] < 0.0)
            wt = INFINITE_DOUBLE;

        edge_t edge = edge_t (from_id, to_id, dist [i], wt,
                edge_id_str, replacement_edges);

        edge_map.emplace (edge_id_str, edge);
        graph::add_to_v2e_map (vert2edge_map, from_id, edge_id_str);
        graph::add_to_v2e_map (vert2edge_map, to_id, edge_id_str);
    }
}

//' identify_graph_components
//'
//' Identify initial graph components for each **vertex**
//' Identification for edges is subsequently perrformed with 
//' \code{rcpp_get_component_vector}.
//'
//' @param v unordered_map <vertex_id_t, vertex_t>
//' @param com component map from each vertex to component numbers
//' @noRd
unsigned int graph::identify_graph_components (vertex_map_t &v,
        std::unordered_map <vertex_id_t, unsigned int> &com)
{
    // initialize components map
    std::unordered_set <vertex_id_t> all_verts;
    for (auto it: v)
        all_verts.emplace (it.first);
    com.clear ();

    std::unordered_set <vertex_id_t> nbs_todo, nbs_done;
    nbs_todo.insert (*all_verts.begin ());
    unsigned int compnum = 0;
    while (all_verts.size () > 0)
    {
        Rcpp::checkUserInterrupt ();
        vertex_id_t vt = (*nbs_todo.begin ());
        all_verts.erase (vt);

        vertex_t vtx = v.find (vt)->second;
        std::unordered_set <vertex_id_t> nbs = vtx.get_all_neighbours ();
        for (auto nvtx: nbs)
        {
            com.emplace (nvtx, compnum);
            if (nbs_done.find (nvtx) == nbs_done.end ())
                nbs_todo.emplace (nvtx);
        }
        nbs_done.emplace (vt);
        com.emplace (vt, compnum);
        nbs_todo.erase (vt);

        if (nbs_todo.size () == 0 && all_verts.size () > 0)
        {
            nbs_todo.insert (*all_verts.begin ());
            compnum++;
        }
    }

    long int largest_id = 0;
    if (compnum > 0)
    {
        std::vector <unsigned int> comp_sizes (compnum + 1, 0);
        for (auto c: com)
            comp_sizes [c.second]++;
        auto maxi = std::max_element (comp_sizes.begin (), comp_sizes.end ());

        largest_id = std::distance (comp_sizes.begin (), maxi);
    }

    return static_cast <unsigned int> (largest_id);
}
