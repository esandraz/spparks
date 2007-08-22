/* ----------------------------------------------------------------------
   SPPARKS - Stochastic Parallel PARticle Kinetic Simulator
   contact info, copyright info, etc
------------------------------------------------------------------------- */

#ifndef APP_LATTICE2D_H
#define APP_LATTICE2D_H

#include "stdio.h"
#include "app.h"

namespace SPPARKS {

class AppLattice2d : public App {
  friend class SweepLattice2d;

 public:
  AppLattice2d(class SPK *, int, char **);
  virtual ~AppLattice2d();
  void init();
  void input(char *, int, char **);
  void run(int, char **);

  double virtual site_energy(int, int) = 0;
  int virtual site_pick_random(int, int, double) = 0;
  int virtual site_pick_local(int, int, double) = 0;
  double virtual site_propensity(int, int, int) = 0;
  void virtual site_event(int, int, int) = 0;
  void virtual site_update_ghosts(int, int) = 0;
  void virtual site_clear_mask(char **, int, int) = 0;

 protected:
  int me,nprocs;
  int ntimestep,seed;
  int dump_style;
  double time,stoptime;
  double stats_time,stats_delta;
  double dump_time,dump_delta;
  double temperature,t_inverse;
  int nsweep;

  int nx_global,ny_global;           // global lattice (0 to nglobal-1)
  int nx_local,ny_local;             // local lattice (1 to nlocal)
                                     // Does not include ghost sites
  int nx_offset,ny_offset;           // global indices of my (1,1) site
  int nx_sector_lo,nx_sector_hi;     // bounds of current sector
  int ny_sector_lo,ny_sector_hi;     // as set by sweeper

  int nxlo,nxhi,nylo,nyhi;           // Upper and lower limits for local lattice
                                     // Includes ghost layer of thickness = delghost
                                     // local sites on 1 to nlocal
  int **lattice;               // owned lattice + ghost lattice
  double *propensity;          // probability for each owned site
  int **ij2site;               // mapping of owned lattice to sites
  int **site2ij;               // mapping of owned sites to lattice indices

  int nx_procs,ny_procs;       // procs in each dim of lattice partition
  int procwest,proceast;       // my neighbor procs
  int procsouth,procnorth;

  double masklimit;            // app-specific, used by sweeper

  int delghost, dellocal;      // App-specific thickness of 
                               // ghost and local layers 
                               // needed for communication.
                               // delghost affects upper and lower
                               // limits for local lattice

  FILE *fp;
  int *ibuf;
  double *dbuf;
  int maxdumpbuf;

  class RandomPark *random;
  class CommLattice2d *comm;

  void virtual input_app(char *, int, char **);
  void virtual init_app() {}

  void iterate();
  void stats();
  void dump_header();
  void dump();
  void dump_lattice();
  void dump_coord();
  virtual void box_bounds(double *, double *, double *, double *);
  virtual void xy(int, int, double *, double *);
  void dump_detailed(char*);
  void dump_detailed_mask(char*,char**);

  void set_stats(int, char **);
  void set_dump(int, char **);
  void set_temperature(int, char **);

  void procs2lattice();
  void ijpbc(int &, int &);
};

// remap i,j indices via PBC if needed

inline void AppLattice2d::ijpbc(int &i, int &j)
{
  if (i < 1) i += nx_local;
  else if (i > nx_local) i -= nx_local;
  if (j < 1) j += ny_local;
  else if (j > ny_local) j -= ny_local;
}

}

#endif