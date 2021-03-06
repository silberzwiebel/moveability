% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/move-stats.R
\name{move_statistics}
\alias{move_statistics}
\title{move_distances}
\usage{
move_statistics(graph, from, quiet = TRUE)
}
\arguments{
\item{graph}{An \code{dodgr_streetnet} object}

\item{from}{Vector of points from which moveability statistics are to be be
calculated.}

\item{quiet}{If \code{FALSE}, display progress messages on screen.}
}
\value{
Vector of moveability values for each point in \code{from}, with
moveability quantified as \code{$m}.
}
\description{
Alias for \link{move_stats}
}
\examples{
# A larger example from the included [hampi()] data.
graph <- dodgr::weight_streetnet (dodgr::hampi)
from <- sample (graph$from_id, size = 100)
d <- move_stats (graph, from = from)
# d is a `data.frame` of the coordinates of all `from` points and
# correponding moveability statisics 
}
