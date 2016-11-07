# include <iostream>
# include <mpi.h>
# ifdef MASTER_NODE
    # include <SFML/Graphics.hpp>
    # include <SFML/Window.hpp>
# endif
# include <boost/cstdint.hpp>

// size of 1 block
# define MPIG_BXLENGTH 2
# define MPIG_BYLENGTH 2
# define MPIG_BZLENGTH 2

// size of xyz in blocks
# define MPIG_UXBCOUNT 200
# define MPIG_UYBCOUNT 200
# define MPIG_UZBCOUNT 24

# define MPIG_PXBCOUNT 2
# define MPIG_PYBCOUNT 2
# define MPIG_PZBCOUNT 1
# define MPIG_PBSPEED 5
# define MPIG_WXBCOUNT 200
# define MPIG_WYBCOUNT 200
# define MPIG_WTITLE "MPI Game"
enum lstate : boost::uint8_t {
    __is_running,
    __not_running
} ;

boost::uint8_t(mpig_mlstate ) = lstate::__not_running;

bool(is_mpig_lstate(boost::uint8_t(__is_type)))
{
    return(mpig_mlstate == __is_type? true : false);
}

const int(pcoordt_elimentc) = 3;
typedef struct __player_coordt {
    int unsigned(xdim) = 0;
    int unsigned(ydim) = 0;
    int unsigned(zdim) = 0;
} player_coordt;

int const(rgba_col_elimentc) = 4;
typedef struct __rgba_colours_t {
    boost::uint8_t(r);
    boost::uint8_t(g);
    boost::uint8_t(b);
    boost::uint8_t(a);
} rgba_colours_t;

int const(uni_par_elimentc) = 4;
typedef struct __uni_particle_t {
    rgba_colours_t(rgba_colours);
    int unsigned(xcoord);
    int unsigned(ycoord);
    int unsigned(zcoord);
} uni_particle_t;

// eatch chunk if prossed by eatch node
// its automaticly slip up into chunks
typedef struct __uni_chunk_t {
    // where the chunk starts and ends
    int unsigned(uni_xpoint[2]) = {0, 0};
    int unsigned(uni_ypoint[2]) = {0, 0};
    int unsigned(uni_zpoint[2]) = {0, 0};
} uni_chunk_t;

MPI_Datatype (mpi_player_coord_t);
MPI_Datatype (mpi_rgba_colours_t);
MPI_Datatype (mpi_uni_particle_t);

int unsigned get_uni_particle_pos(int unsigned xlength, int unsigned ylength, int unsigned zlength, int unsigned x, int unsigned y, int unsigned z)
{
    return((x + (y * xlength) + (z * (xlength * ylength))));
}

int (main( ) )
{
    MPI_Init (NULL, NULL);

    int(network_node_rank);
    MPI_Comm_rank (MPI_COMM_WORLD, & network_node_rank);

    int(network_node_count);
    MPI_Comm_size (MPI_COMM_WORLD, & network_node_count);
 
    int unsigned(slave_count ) = (network_node_count - 1);

    int(* * eliment_lengths) = new int * [3];
    eliment_lengths[0] = new int[3];
    eliment_lengths[1] = new int[4];
    eliment_lengths[2] = new int[4];

    MPI_Datatype(* * eliment_types) = new MPI_Datatype * [3];
    eliment_types[0] = new MPI_Datatype [3];
    eliment_types[1] = new MPI_Datatype [4];
    eliment_types[2] = new MPI_Datatype [4];

    MPI_Aint(* * eliment_offsets) = new MPI_Aint * [3];
    eliment_offsets[0] = new MPI_Aint [3];
    eliment_offsets[1] = new MPI_Aint [4];
    eliment_offsets[2] = new MPI_Aint [4];

    eliment_lengths[0][0] = 1;
    eliment_lengths[0][1] = 1;
    eliment_lengths[0][2] = 1;
    eliment_types[0][0] = MPI_UNSIGNED;
    eliment_types[0][1] = MPI_UNSIGNED;
    eliment_types[0][2] = MPI_UNSIGNED;
    eliment_offsets[0][0] = offsetof(player_coordt, xdim);
    eliment_offsets[0][1] = offsetof(player_coordt, ydim);
    eliment_offsets[0][2] = offsetof(player_coordt, zdim);
 
    eliment_lengths[1][0] = 1;
    eliment_lengths[1][1] = 1;
    eliment_lengths[1][2] = 1;
    eliment_lengths[1][3] = 1;
    eliment_types[1][0] = MPI_UINT8_T;
    eliment_types[1][1] = MPI_UINT8_T;
    eliment_types[1][2] = MPI_UINT8_T;
    eliment_types[1][3] = MPI_UINT8_T;
    eliment_offsets[1][0] = offsetof(rgba_colours_t, r);
    eliment_offsets[1][1] = offsetof(rgba_colours_t, g);
    eliment_offsets[1][2] = offsetof(rgba_colours_t, b);
    eliment_offsets[1][3] = offsetof(rgba_colours_t, a);

    MPI_Type_create_struct (pcoordt_elimentc, eliment_lengths[0], eliment_offsets[0], eliment_types[0], & mpi_player_coord_t);
    MPI_Type_create_struct (rgba_col_elimentc, eliment_lengths[1], eliment_offsets[1], eliment_types[1], & mpi_rgba_colours_t);
    MPI_Type_commit (& mpi_player_coord_t);
    MPI_Type_commit (& mpi_rgba_colours_t);

    eliment_lengths[2][0] = 1;
    eliment_lengths[2][1] = 1;
    eliment_lengths[2][2] = 1;
    eliment_lengths[2][3] = 1;
    eliment_types[2][0] = mpi_rgba_colours_t;
    eliment_types[2][1] = MPI_UNSIGNED;
    eliment_types[2][2] = MPI_UNSIGNED;
    eliment_types[2][3] = MPI_UNSIGNED;
    eliment_offsets[2][0] = offsetof(uni_particle_t, rgba_colours);
    eliment_offsets[2][1] = offsetof(uni_particle_t, xcoord);
    eliment_offsets[2][2] = offsetof(uni_particle_t, ycoord);
    eliment_offsets[2][3] = offsetof(uni_particle_t, zcoord);

    MPI_Type_create_struct (uni_par_elimentc, eliment_lengths[2], eliment_offsets[2], eliment_types[2], & mpi_uni_particle_t);
    MPI_Type_commit (& mpi_uni_particle_t);
    
    player_coordt (player_coordinates);

    player_coordinates.xdim = 0;
    player_coordinates.ydim = 0;
    player_coordinates.zdim = 0;
# ifdef MASTER_NODE

    struct __camera_coordinates {
        int unsigned(xdim) = 0;
        int unsigned(ydim) = 0;
    } camera_coordinates;

# endif
    int unsigned(uni_xlength) = (MPIG_UXBCOUNT * MPIG_BXLENGTH);
    int unsigned(uni_ylength) = (MPIG_UYBCOUNT * MPIG_BYLENGTH);
    int unsigned(uni_zlength) = (MPIG_UZBCOUNT * MPIG_BZLENGTH);

    int unsigned(chunk_xlength) = uni_xlength / slave_count;
    int unsigned(chunk_ylength) = uni_ylength / slave_count;
    int unsigned(chunk_zlength) = uni_zlength / slave_count;

    int unsigned(player_xlength) = (MPIG_PXBCOUNT * MPIG_BXLENGTH);
    int unsigned(player_ylength) = (MPIG_PYBCOUNT * MPIG_BYLENGTH);
    int unsigned(player_zlength) = (MPIG_PZBCOUNT * MPIG_BZLENGTH);
    
    int unsigned(total_chunk_size) = (chunk_xlength * chunk_ylength * chunk_zlength);

    int unsigned(uni_particle_count) = (uni_xlength * uni_ylength * uni_zlength);

    int unsigned(uni_chunk_count) = (slave_count * slave_count * slave_count);
    uni_chunk_t(* universe_chunks) = new uni_chunk_t [uni_chunk_count]; 
/*
    int unsigned x, y, z;
    for (int unsigned i = 0; i != slave_count; i ++)
    {
        universe_chunks[i].uni_xpoint[0] = 0;
        universe_chunks[i].uni_ypoint[0] = 0;
        universe_chunks[i].uni_zpoint[0] = 0;

        universe_chunks[i].uni_xpoint[1] = (chunk_xlength - 1);
        universe_chunks[i].uni_ypoint[1] = (chunk_ylength - 1);
        universe_chunks[i].uni_zpoint[1] = (chunk_zlength - 1);

        if (i )
*/
/*        if (x == 0) {
            universe_chunks[x].uni_xpoint[0] = 0;
            universe_chunks[x].uni_ypoint[0] = 0;
            universe_chunks[x].uni_zpoint[0] = 0;

            universe_chunks[x].uni_xpoint[1] = chunk_xlength-1;
            universe_chunks[x].uni_ypoint[1] = chunk_ylength-1;
            universe_chunks[x].uni_zpoint[1] = chunk_zlength-1;
        } else {
            universe_chunks[x].uni_xpoint[0] = universe_chunks[x-1].uni_xpoint[1] + 1;
            universe_chunks[x].uni_ypoint[0] = universe_chunks[x-1].uni_ypoint[1] + 1;
            universe_chunks[x].uni_zpoint[0] = universe_chunks[x-1].uni_zpoint[1] + 1;

            universe_chunks[x].uni_xpoint[1] = universe_chunks[x-1].uni_xpoint[1] + (chunk_xlength);
            universe_chunks[x].uni_ypoint[1] = universe_chunks[x-1].uni_ypoint[1] + (chunk_ylength);
            universe_chunks[x].uni_zpoint[1] = universe_chunks[x-1].uni_zpoint[1] + (chunk_zlength);  
        }
        //std::cout << universe_chunks[x].uni_xpoint[0] << "#" << universe_chunks[x].uni_xpoint[1]<< std::endl;
*/
    

//    for (int unsigned z = 0; z != slave_count; z ++)
//    {
        
//    }
//    if (network_node_rank == 0)
//   {

    for (int unsigned z = 0; z != slave_count; z ++)
    {
        int unsigned __z = (z * chunk_zlength);

        for (int unsigned y = 0; y != slave_count; y ++)
        {
            int unsigned __y = (y * chunk_ylength);

            for (int unsigned x = 0; x != slave_count; x ++)
            {
                int unsigned __x = (x * chunk_xlength); 
                //int unsigned __y = (y * chunk_ylength);
                //int unsigned __z = (z * chunk_zlength);
                int unsigned pos = x + (y * slave_count) + (z * (slave_count * slave_count));            
                universe_chunks[pos].uni_xpoint[0] = __x;       
                universe_chunks[pos].uni_xpoint[1] = (universe_chunks[pos].uni_xpoint[0] + (chunk_xlength - 1));
            
                universe_chunks[pos].uni_ypoint[0] = __y;
                universe_chunks[pos].uni_ypoint[1] = (universe_chunks[pos].uni_ypoint[0] + (chunk_ylength - 1));

                universe_chunks[pos].uni_zpoint[0] = __z;
                universe_chunks[pos].uni_zpoint[1] = (universe_chunks[pos].uni_zpoint[0] + (chunk_zlength - 1));

//                std::cout << ((universe_chunks[pos].uni_xpoint[1] + 1) - universe_chunks[pos].uni_xpoint[0]) *
//                ((universe_chunks[pos].uni_ypoint[1] + 1) - universe_chunks[pos].uni_ypoint[0]) *
//                ((universe_chunks[pos].uni_zpoint[1] + 1) - universe_chunks[pos].uni_zpoint[0]) << std::endl;

   //             std::cout << universe_chunks[pos].uni_xpoint[0] << " / " << universe_chunks[pos].uni_xpoint[1] << std::endl;
     //          std::cout << universe_chunks[pos].uni_ypoint[0] << " / " << universe_chunks[pos].uni_ypoint[1] << std::endl;
       //         std::cout << universe_chunks[pos].uni_zpoint[0] << " / " << universe_chunks[pos].uni_zpoint[1] << std::endl << std::endl;
            }
        }  
    }
//}
    //std::cout << total_chunk_size << std::endl;
//    int id = 3;
//    std::cout << 
//    (universe_chunks[id].uni_xpoint[1] + 1) * 
//    (universe_chunks[id].uni_ypoint[1] + 1) *
//    (universe_chunks[id].uni_zpoint[1] + 1) / 3 << std::endl; 
    //int x = 1;
    //std::cout << universe_chunks[x].uni_xpoint[0] << "#" << universe_chunks[x].uni_xpoint[1]<< std::endl;
 //   MPI_Finalize( );
  //  return 0;
    uni_particle_t(* uni_particles) = new uni_particle_t [uni_particle_count];
   
# ifdef MASTER_NODE
    int unsigned(win_xlength ) = (MPIG_WXBCOUNT * MPIG_BXLENGTH);
    int unsigned(win_ylength ) = (MPIG_WYBCOUNT * MPIG_BYLENGTH);
    int unsigned anix_e = 50;
    int unsigned aniy_e = 50;

    int unsigned anix = 0, aniy = 0, anix_r = anix_e, aniy_r = aniy_e;
    boost::uint8_t(* window_pixcbuff) = new boost::uint8_t [(win_xlength * win_ylength)* 4];

    sf::RenderWindow window_instance(sf::VideoMode(win_xlength, win_ylength), MPIG_WTITLE);

    sf::Texture window_ctexture;
    window_ctexture.create(win_xlength, win_ylength);
    sf::Sprite window_csprite(window_ctexture);
# endif
    boost::uint8_t(is_inside) = false;

    if (network_node_rank == 0)
    {
        mpig_mlstate = lstate::__is_running;

        for (int unsigned(node_rank ) = 1; node_rank != (slave_count + 1); node_rank ++)
            MPI_Send (& mpig_mlstate, 1, MPI_UINT8_T, node_rank, 0, MPI_COMM_WORLD);
        do
        {
          //for(int x = 0; x != 100000000; x ++) {}

            for (int unsigned(node_rank ) = 1; node_rank != (slave_count + 1); node_rank ++)
                MPI_Send (& player_coordinates, 1, mpi_player_coord_t, node_rank, 0, MPI_COMM_WORLD);

            for (int unsigned(node_rank ) = 1; node_rank != (slave_count + 1); node_rank ++)
            {
                for (int unsigned i = 1; i != slave_count+1; i ++) {
                for (int unsigned ii = 1; ii != slave_count+1; ii ++) {
                MPI_Recv (& is_inside, 1, MPI_UINT8_T, node_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //if (is_inside == true) std::cout << "SU" << std::endl;
                //std::cout << "Getting from Node: " << node_rank  << std::endl;
                if (is_inside == true)
                {
            //        std::cout << "NODE: " << node_rank << std::endl;
                    for (int unsigned o = 0; o != total_chunk_size; o ++)
                    {
                        //std::cout << "NODE: " << node_rank  << std::endl;
                        uni_particle_t particle;
                        //std::cout << "Getting from Node: " << node_rank  << std::endl;
                        MPI_Recv (& particle, 1, mpi_uni_particle_t, node_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                        //std::cout << "NODE: " << node_rank  << std::endl;
                        int unsigned par_pos = get_uni_particle_pos(uni_xlength, uni_ylength, uni_zlength, 
                        particle.xcoord, particle.ycoord, particle.zcoord);
                        //std::cout << "Getting from Node: " << par_pos << " / " << total_chunk_size << std::endl;
                        //if (node_rank == 2)
                        //    std::cout << "X: " << particle.xcoord << ", Y: " << particle.ycoord << ", Z: " << particle.zcoord << std::endl;
                        //std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>" << particle.xcoord << ", T: " << uni_particle_count << std::endl;
                        uni_particles[par_pos] = particle;
                    }
                } else
                {
              //      std::cout << "FALSE" << std::endl;
                }
                //std::cout << "NODE_: " << node_rank  << std::endl;
               }}
            }
    //        std::cout << "TICKING" << std::endl;
# ifdef MASTER_NODE
            for (int unsigned(y ) = 0; y != win_ylength; y ++)
            {
                for (int unsigned(x ) = 0; x != win_xlength; x ++)
                {
                    //int unsigned o = ((x * 4) + ((y * win_xlength) * 4));
                    int unsigned o = ((x + (y * win_xlength)) * 4);
                    int unsigned part_pos = get_uni_particle_pos(uni_xlength, uni_ylength, uni_zlength, x, y, 0);
                    //std::cout << o << std::endl;
                    window_pixcbuff[o] = uni_particles[part_pos].rgba_colours.r;
                    window_pixcbuff[o + 1] = uni_particles[part_pos].rgba_colours.g;
                    window_pixcbuff[o + 2] = uni_particles[part_pos].rgba_colours.b;
                    window_pixcbuff[o + 3] = uni_particles[part_pos].rgba_colours.a;
                    //std::cout << unsigned(uni_particles[part_pos].rgba_colours.b) << std::endl;
                }

            }

            if (! window_instance.isOpen( ) )
                mpig_mlstate = lstate::__not_running;

            sf::Event window_event;
            while (window_instance.pollEvent (window_event))
                if (window_event.type == sf::Event::Closed)
                    window_instance.close( );
# endif
            //std::cout << "pX: " << player_coordinates.xdim << ", pY: " << player_coordinates.ydim << ", pZ: " << player_coordinates.zdim << std::endl;
      
# ifdef MASTER_NODE
            //std::cout << "pX: " << player_coordinates.xdim << ", pY: " << player_coordinates.ydim << ", pZ: " << player_coordinates.zdim << std::endl;
            //std::cout << "cX: " << camera_coordinates.xdim << ", cY: " << camera_coordinates.ydim << std::endl;

            window_ctexture.update(window_pixcbuff);

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::W))
                if ((player_coordinates.zdim + player_zlength) < (uni_zlength-1))
                    ++ player_coordinates.zdim;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::S))
                if (player_coordinates.zdim > 0) -- player_coordinates.zdim;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Numpad6))
                if ((player_coordinates.xdim + player_xlength) < (uni_xlength-1))
                    player_coordinates.xdim += MPIG_PBSPEED;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Numpad4)) 
            {
                if (player_coordinates.xdim > 0)
                    player_coordinates.xdim -= MPIG_PBSPEED; 
                else player_coordinates.xdim = 0;
            }
            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Numpad2))
                if ((player_coordinates.ydim + player_ylength) < (uni_ylength-1))
                    player_coordinates.ydim += MPIG_PBSPEED;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Numpad8))
            {
                if (player_coordinates.ydim > 0) 
                    player_coordinates.ydim -= MPIG_PBSPEED; 
                else player_coordinates.ydim = 0;
            }
            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right))
                ++ camera_coordinates.xdim;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left))
                -- camera_coordinates.xdim;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up))
                ++ camera_coordinates.ydim;

            if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down))
                -- camera_coordinates.ydim;

            if (anix < anix_e)
            {
                player_coordinates.xdim += MPIG_PBSPEED;
                anix++;
            }
            else
            {
                if (aniy < aniy_e)
                {
                    player_coordinates.ydim += MPIG_PBSPEED;
                    aniy ++;
                }
                else
                {
                    if (anix_r != 0)
                    {
                        player_coordinates.xdim -= MPIG_PBSPEED;
                        anix_r --;
                    }
                    else
                    {
                        if (aniy_r != 0)
                        {
                            player_coordinates.ydim -= MPIG_PBSPEED;
                            aniy_r --;
                        }
                        else
                        {
                            anix = 0;
                            aniy = 0;
                            anix_r = anix_e;
                            aniy_r = aniy_e;
                        }
                    }
                }
            }

            window_instance.clear( );
            window_instance.draw (window_csprite);
            window_instance.display( );
# endif
        for (int unsigned(node_rank ) = 1; node_rank != (slave_count + 1); node_rank ++)
            MPI_Send (& mpig_mlstate, 1, MPI_UINT8_T, node_rank, 0, MPI_COMM_WORLD);

        } while(is_mpig_lstate (lstate::__is_running) == true);
    }
    else
    {
        boost::uint8_t x = false;
        boost::uint8_t y = false;
        boost::uint8_t z = false;
       
        if (is_mpig_lstate (lstate::__not_running) == true)
            MPI_Recv (& mpig_mlstate, 1, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        do
        {
            MPI_Recv (& player_coordinates, 1, mpi_player_coord_t, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int unsigned i = 0; i != slave_count; i ++) {    
            for (int unsigned ii = 0; ii != slave_count; ii ++) {
           //int unsigned pos = x + (y * slave_count) + (z * (slave_count * slave_count));
        
            //int unsigned chunkpos = ((network_node_rank-1) * slave_count) + i;
            int unsigned chunkpos = ii + (i * slave_count) + ((network_node_rank -1) * (slave_count * slave_count));
            //std::cout << "N: " << network_node_rank << ", CHUNK_POS: " << chunkpos << ", X: " << i << ", Y: " << ii << ", Z: " << (network_node_rank -1) << std::endl;
            //d::cout << "CHUNK POS: "  << chunkpos << ", ND: "<< (network_node_rank-1) * slave_count << ", S: " << network_node_rank-1 << std::endl << std::endl  << std::endl ; 
            int unsigned pos = chunkpos;
            //std::cout << "NODE: " << network_node_rank << ", " << universe_chunks[chunkpos].uni_xpoint[0] << ", " << universe_chunks[chunkpos].uni_ypoint[0] << ", " << universe_chunks[chunkpos].uni_zpoint[0] << std::endl; 

//            std::cout << ((universe_chunks[pos].uni_xpoint[1] + 1) - universe_chunks[pos].uni_xpoint[0]) *
//                ((universe_chunks[pos].uni_ypoint[1] + 1) - universe_chunks[pos].uni_ypoint[0]) *
//                ((universe_chunks[pos].uni_zpoint[1] + 1) - universe_chunks[pos].uni_zpoint[0]) << std::endl;

            //x = false, y = false, y = false;

            if ((((player_coordinates.xdim >= universe_chunks[chunkpos].uni_xpoint[0]) || (player_coordinates.xdim + player_xlength >= universe_chunks[chunkpos].uni_xpoint[0])) &&
                (player_coordinates.xdim + player_xlength) <= universe_chunks[chunkpos].uni_xpoint[1])) x = true; else x = false;
//            std::cout << ">>>>>>>>>>;" << x << ", NODE: " << network_node_rank << std::endl;
            //x = false; 

            if ((((player_coordinates.ydim >= universe_chunks[chunkpos].uni_ypoint[0]) || (player_coordinates.ydim + player_ylength >= universe_chunks[chunkpos].uni_ypoint[0])) && 
                (player_coordinates.ydim + player_ylength) <= universe_chunks[chunkpos].uni_ypoint[1])) y = true; else y = false;

            if ((((player_coordinates.zdim >= universe_chunks[chunkpos].uni_zpoint[0]) || (player_coordinates.zdim + player_zlength >= universe_chunks[chunkpos].uni_zpoint[0]))  &&
                (player_coordinates.zdim + player_zlength) <= universe_chunks[chunkpos].uni_zpoint[1])) z = true; else z = false;

            //std::cout << "x:" << unsigned(x) << ", y:" << unsigned(y) << ", z:" << unsigned(z) << ", NODE: " << (network_node_rank -1) 
            //<< ", CHUNK: "  << chunkpos << std::endl;
            //x = false, y = false, y = false;

            //x = false;
            //y = false;
            //z = false;
     
            if (x && y && z)
            {
                //std::cout << "CHUNK POS: "  << chunkpos << ", ND: "<< (network_node_rank-1) * slave_count << ", S: " << network_node_rank-1 << std::endl << std::endl  << std::endl ; 
                is_inside = true;
                MPI_Send (& is_inside, 1, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD);


                for (int unsigned z = universe_chunks[chunkpos].uni_zpoint[0]; z != universe_chunks[chunkpos].uni_zpoint[1]+1; z ++)
                {
                    for (int unsigned y = universe_chunks[chunkpos].uni_ypoint[0]; y != universe_chunks[chunkpos].uni_ypoint[1]+1; y ++)
                    {
                        for (int unsigned x = universe_chunks[chunkpos].uni_xpoint[0]; x != universe_chunks[chunkpos].uni_xpoint[1]+1; x ++)
                        {
                            uni_particle_t particle; 
                            particle.rgba_colours.r = 0;
                            particle.rgba_colours.g = 255;
                            particle.rgba_colours.b = 0;
                            if ((network_node_rank -1) == 1)
                                particle.rgba_colours.b = 255;
                            particle.rgba_colours.a = 255;
                            particle.xcoord = x;
                            particle.ycoord = y;
                            particle.zcoord = z;


                            if (z >= player_coordinates.zdim && z <= (player_coordinates.zdim + player_zlength))
                            {
                                if (y >= player_coordinates.ydim && y <= (player_coordinates.ydim + player_ylength))
                                {
                                    if (x >= player_coordinates.xdim && x <= (player_coordinates.xdim + player_xlength))
                                    {
                                        particle.rgba_colours.r = 255;
                                        particle.rgba_colours.g = 0;
                                        particle.rgba_colours.b = 0;
                                        particle.rgba_colours.a = 255;
                                    }
                                }
                            }
                            //std::cout << z << std::endl;
                            MPI_Send (& particle, 1, mpi_uni_particle_t, 0, 0, MPI_COMM_WORLD);
                        }
                    }
                }

            //std::cout << "x:" << unsigned(x) << ", y:" << unsigned(y) << ", z:" << unsigned(z) << ", NODE: " << (network_node_rank -1) << std::endl;    
            } else
            {
                is_inside = false;
                MPI_Send (& is_inside, 1, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD);
            }
        }}
            //std::cout << "x:" << unsigned(x) << ", y:" << unsigned(y) << ", z:" << unsigned(z) << ", NODE: " << (network_node_rank -1) << std::endl; 
            
            MPI_Recv (& mpig_mlstate, 1, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);    
        } while(is_mpig_lstate (lstate::__is_running) == true);
    } 

    MPI_Finalize( );
}
