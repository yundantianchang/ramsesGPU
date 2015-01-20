/**
 * \file MpiCommCart.h
 * \brief A simple derive class of MpiComm to handle cartesian
 * topology.
 * 
 * \date 5 Oct 2010
 * \author Pierre Kestener
 *
 * $Id: MpiCommCart.h 1783 2012-02-21 10:20:07Z pkestene $
 */
#ifndef MPI_COMM_CART_H_
#define MPI_COMM_CART_H_

#include "MpiComm.h"

namespace hydroSimu {

  //! defgroup mpi_cartesian
  //!@{
  //! number of dimensions of the cartesian virtual topology
  enum {
    NDIM_2D = 2,
    NDIM_3D = 3
  };

  //! do we allow processor reordering by the MPI cartesian communicator ?
  enum {
    MPI_REORDER_FALSE = 0,
    MPI_REORDER_TRUE  = 1
  };
  
  //! should the cartesian virtual topology be considered periodic ?
  enum {
    MPI_CART_PERIODIC_FALSE = 0,
    MPI_CART_PERIODIC_TRUE  = 1
  };
  
  //! MPI topology directions
  enum {
    MPI_TOPO_DIR_X = 0,
    MPI_TOPO_DIR_Y = 1,
    MPI_TOPO_DIR_Z = 2
  };

  //! MPI topology shift direction
  enum {
    MPI_SHIFT_NONE = 0,
    MPI_SHIFT_FORWARD = 1
  };

  //! identifying neighbors
  enum NeighborLocation {
    X_MIN = 0,
    X_MAX = 1,
    Y_MIN = 2,
    Y_MAX = 3,
    Z_MIN = 4,
    Z_MAX = 5
  };

  //! number of neighbors
  enum {
    N_NEIGHBORS_2D = 4,
    N_NEIGHBORS_3D = 6
  };

  //! direction
  enum Dir {
    DIR_X=0, 
    DIR_Y=1, 
    DIR_Z=2
  };

  //!@}

  /**
   * \brief Object representation of an MPI communicator with
   * cartesian virtual topology.
   */
  class MpiCommCart : public MpiComm
  {
  public:
    //! Construct a MpiCommCart using a 2D cartesian virtual topology
    MpiCommCart(int mx, int my, int isPeriodic, int allowReorder);

    //! Construct a MpiCommCart using a 3D cartesian virtual topology
    MpiCommCart(int mx, int my, int mz, int isPeriodic, int allowReorder);
    
    //! Destructor
    virtual ~MpiCommCart();

  private:
    int mx_, my_, mz_;
    int *myCoords_;
    bool is2D;

  public:
    //! get topology dimension (2 or 3) from MPI_Cartdim_get just for cross-checking
    virtual int getDim() const;
    
    //! use MPI_Cart_rank to get rank of the shifted process
    virtual int getCartRank(const int coords[]) const;
    
    //! return cartesian coordinate of a process of a given rank
    virtual void getCoords(int rank, int maxdims, int coords[]) const;
    
    //! return my coordinates
    virtual void getMyCoords(int coords[]);

    //! use MPI_Cart_shift to get rank of the shifted processes
    //! regarding the given direction
    virtual void shift(int direction, int disp,
		       int &rank_source, int &rank_dest) const;

    //! return rank of the neighbor process identified by the template parameter
    template <NeighborLocation nl>
    int getNeighborRank() const;
  };

  // =======================================================
  // =======================================================  
  inline int
  MpiCommCart::getDim() const 
  {
    int ndims;
    errCheck( MPI_Cartdim_get(comm_, &ndims), "MPI_Cartdim_get" );
    return ndims;
  }
  
  // =======================================================
  // =======================================================  
  inline int
  MpiCommCart::getCartRank(const int coords[]) const 
  {
    int rank;
    errCheck( MPI_Cart_rank(comm_, const_cast<int *>(coords), &rank), "MPI_Cart_rank" );
    return rank;
  }

  // =======================================================
  // =======================================================  
  inline void
  MpiCommCart::getCoords(int rank, int maxdims, int coords[]) const 
  {
    errCheck( MPI_Cart_coords(comm_, rank, maxdims, coords), "MPI_Cart_coords");
  } 
  
  // =======================================================
  // =======================================================  
  inline void
  MpiCommCart::getMyCoords(int coords[])
  {
    int maxDim = ( is2D ? 2 : 3);
    for (int i=0; i<maxDim; ++i) {
      coords[i] = myCoords_[i];
    }
  } 
  
  // =======================================================
  // =======================================================  
  inline void
  MpiCommCart::shift(int direction, int disp,
		     int &rank_source, int &rank_dest) const 
  {
    errCheck( MPI_Cart_shift(comm_, direction, disp, &rank_source, &rank_dest), "MPI_Cart_shift");
  }

  // =======================================================  
  // =======================================================  
  template <NeighborLocation nl>
  int MpiCommCart::getNeighborRank() const
  {
    
    int rank;

    if (is2D) { // 2D
      int coords[NDIM_2D];
      coords[0] = myCoords_[0];
      coords[1] = myCoords_[1];

      if (nl == X_MIN) {
	coords[0] -= 1;
      } else if (nl == X_MAX) {
	coords[0] += 1;
      } else if (nl == Y_MIN) {
	coords[1] -= 1;
      } else if (nl == Y_MAX) {
	coords[1] += 1;
      }
      rank = getCartRank(coords);

    } else { // 3D
      int coords[NDIM_3D];
      coords[0] = myCoords_[0];
      coords[1] = myCoords_[1];
      coords[2] = myCoords_[2];
    
      if (nl == X_MIN) {
	coords[0] -= 1;
      } else if (nl == X_MAX) {
	coords[0] += 1;
      } else if (nl == Y_MIN) {
	coords[1] -= 1;
      } else if (nl == Y_MAX) {
	coords[1] += 1;
      } else if (nl == Z_MIN) {
	coords[2] -= 1;
      } else if (nl == Z_MAX) {
	coords[2] += 1;
      }
      rank = getCartRank(coords);

    }

    return rank;
  }
  
} // namespace hydroSimu

#endif // MPI_COMM_CART_H_
