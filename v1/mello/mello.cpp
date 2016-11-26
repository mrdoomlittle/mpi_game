# ifdef CORE_NODE
    # include <SFML/Graphics.hpp>
    # include <SFML/Window.hpp>
# endif
# include <boost/cstdint.hpp>
# include <boost/cstdlib.hpp>
# include <boost/mpi/environment.hpp>
# include <boost/mpi/communicator.hpp>
//# include <boost/array.hpp>
//# include <boost/foreach.hpp>
//# include <boost/numeric/ublas/vector.hpp>
# define UNIBLOCK_XDIM_LEN 16
# define UNIBLOCK_YDIM_LEN 16
# define CORE_NODE_ID 0

// this would be the same as the other one but using Boost MPI
// this is trial and error code as theres alot of problums with it
# include <iostream>
struct rgba_colour_t {
    boost::uint8_t r, g, b, a;
    private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & r;
        ar & g;
        ar & b;
        ar & a;
    }
} ;

template <typename dim_t>
struct coordinates_t {
    dim_t xdim, ydim, zdim;
    private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & xdim;
        ar & ydim;
        ar & zdim;
    }
} ;

struct uni_chunk_t {
    boost::uint16_t xdim_start, ydim_start;
    rgba_colour_t * chunk_data;
} ;
# define PLAYER_PIX_PER_TICK 5
int main(int arg_count, char * arg_list[])
{
    boost::mpi::environment environment;
    boost::mpi::communicator communicator;
    boost::uint8_t amount_of_nodes = communicator.size();
    boost::uint8_t amount_of_slaves = (amount_of_nodes - 1);

    if (arg_count != 7) return 1;

    boost::uint8_t uni_chunk_split_ratio = 1;

    boost::uint16_t uni_xdim_len = (std::atoi(arg_list[1]) * UNIBLOCK_XDIM_LEN);
    boost::uint16_t uni_ydim_len = (std::atoi(arg_list[2]) * UNIBLOCK_YDIM_LEN);
    //boost::uint16_t cam_xdim_len = std::atoi(argv[3]);
    //boost::uint16_t cam_ydim_len = std::atoi(argv[4]);
    boost::uint16_t cam_xdim_len = std::atoi(arg_list[3]);
    boost::uint16_t cam_ydim_len = std::atoi(arg_list[4]);
    coordinates_t<boost::uint16_t> cam_coords = {0, 0, 0};

    boost::uint16_t player_xdim_len = std::atoi(arg_list[5]);
    boost::uint16_t player_ydim_len = std::atoi(arg_list[6]);
    coordinates_t<boost::uint16_t> player_coords = {0, 0, 0};
   
    int unsigned uni_chunk_xdim_len = (uni_xdim_len / (uni_chunk_split_ratio * amount_of_slaves));
    int unsigned uni_chunk_ydim_len = (uni_ydim_len / (uni_chunk_split_ratio * amount_of_slaves)); 

    int unsigned uni_chunk_size = (uni_chunk_xdim_len * uni_chunk_ydim_len);

    boost::uint8_t slave_nids[amount_of_slaves] = {0};

    boost::uint8_t apos = 0;
    for (boost::uint8_t i = 0; i != amount_of_nodes; i ++)
        if (i != CORE_NODE_ID) { slave_nids[apos] = i; apos ++; }

    uni_chunk_t uni_chunks[amount_of_slaves][amount_of_slaves];   
    for (int unsigned y = 0; y != amount_of_slaves; y ++) {
        for (int unsigned x = 0; x != amount_of_slaves; x ++) {
            uni_chunks[y][x].xdim_start = (x * uni_chunk_xdim_len);
            uni_chunks[y][x].ydim_start = (y * uni_chunk_ydim_len);
            uni_chunks[y][x].chunk_data = new rgba_colour_t[uni_chunk_size];
        }
    }

    if (communicator.rank() == CORE_NODE_ID)
        for (int unsigned y = 0; y != amount_of_slaves; y ++) 
            for (int unsigned x = 0; x != amount_of_slaves; x ++)
                std::cout << "x_start: " << uni_chunks[y][x].xdim_start << ", y_start: " << uni_chunks[y][x].ydim_start << std::endl;
    //coordinates_t<boost::uint16_t> 

 
    enum loop_state : boost::uint8_t {
        __is_running,
        __is_not_running
    } ;
  
# ifdef CORE_NODE
    bool gloop_state = loop_state::__is_running; 

    sf::RenderWindow w_instance(sf::VideoMode(cam_xdim_len, cam_ydim_len), "MPI Game V1");
    
    boost::uint8_t cam_colour_buffer[(cam_xdim_len * cam_ydim_len) * 4] = {0};

    sf::Texture w_texture;
    w_texture.create(cam_xdim_len, cam_ydim_len);
    sf::Sprite w_sprite(w_texture);
# else
    bool gloop_state = loop_state::__is_not_running;
# endif
    if (communicator.rank() == CORE_NODE_ID)
    {
        for (boost::uint8_t i = 0; i != amount_of_slaves; i ++)
            communicator.send(slave_nids[i], 0, gloop_state);
        do
        {
            for (boost::uint8_t i = 0; i != amount_of_slaves; i ++)
                communicator.send(slave_nids[i], 0, player_coords);
# ifdef CORE_NODE
            memset(cam_colour_buffer, 0, ((cam_xdim_len * cam_ydim_len) * 4));
            //cam_colour_buffer = {0};
# endif
            boost::uint8_t is_inside_chunk[amount_of_slaves] = {0};
            for (boost::uint8_t i = 0; i != amount_of_slaves; i ++)
            {
                communicator.recv(slave_nids[i], 0, is_inside_chunk, amount_of_slaves);
                for (boost::uint8_t o = 0; o != amount_of_slaves; o ++)
                {
                    std::cout << ":" <<unsigned(is_inside_chunk[o]);
                   
                    if (is_inside_chunk[o] == 1)
                    {
                        //std::cout << "found in y: " << unsigned(i) << " and x: " << unsigned(o)  << std::endl;
                
                        communicator.recv(slave_nids[i], 0, uni_chunks[i][o].chunk_data, uni_chunk_size);

# ifdef CORE_NODE
                        int unsigned pix = 0;
                         for (int unsigned pixy = 0; pixy != uni_chunk_ydim_len; pixy ++) {
                        for (int unsigned pixx = 0; pixx != uni_chunk_xdim_len; pixx ++) {
                            //int unsigned pixpos = pix + (i * (amount_of_slaves * uni_chunk_size)) + (o * uni_chunk_size);
                            int unsigned pixpos = pixx + (pixy * cam_xdim_len) + (o * uni_chunk_xdim_len) + (i * (amount_of_slaves * uni_chunk_size));
                            
                            pixpos *= 4;
                            cam_colour_buffer[pixpos] = uni_chunks[i][o].chunk_data[pix].r;
                            cam_colour_buffer[pixpos+1] = uni_chunks[i][o].chunk_data[pix].g;
                            cam_colour_buffer[pixpos+2] = uni_chunks[i][o].chunk_data[pix].b;
                            cam_colour_buffer[pixpos+3] = uni_chunks[i][o].chunk_data[pix].a;
//                            std::cout << pixpos << "SIZE OF CHUNK: " << uni_chunk_size << std::endl;
                            pix++;
                        }}
                        is_inside_chunk[o] = 0;

# endif
                    }
                }
                std::cout << "\n";
            }

            
/*
        for (int unsigned y = 0; y != amount_of_slaves; y ++) {
            for (int unsigned x = 0; x != amount_of_slaves; x ++) {
                for (int unsigned pix = 0; pix != uni_chunk_size; pix ++) {
                    
                }
            }
        }
*/       
# ifdef CORE_NODE
            w_texture.update(cam_colour_buffer);

            if (! w_instance.isOpen())
                gloop_state = loop_state::__is_not_running;

            sf::Event w_event;
            while (w_instance.pollEvent(w_event))
                if (w_event.type == sf::Event::Closed)
                    w_instance.close();

# endif

//for (int unsigned x = 0; x != 10000000; x ++){}

# ifdef CORE_NODE
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                if (player_coords.ydim > 0) player_coords.ydim -= PLAYER_PIX_PER_TICK;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                if ((player_coords.ydim + player_ydim_len) < uni_ydim_len)
                    player_coords.ydim += PLAYER_PIX_PER_TICK;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                if ((player_coords.xdim + player_xdim_len) < uni_xdim_len)
                    player_coords.xdim += PLAYER_PIX_PER_TICK;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                if (player_coords.xdim > 0) player_coords.xdim -= PLAYER_PIX_PER_TICK;

            w_instance.clear();
            w_instance.draw(w_sprite);
            w_instance.display();
# endif
            for (boost::uint8_t i = 0; i != amount_of_slaves; i ++)
                communicator.send(slave_nids[i], 0, gloop_state);
        } while (gloop_state == loop_state::__is_running);
    }
    else
    {
        boost::uint8_t current_rank_id = communicator.rank()-1;
       
        //boost::uint16_t y_ = 0
        //boost::uint8_t is_inside_chunk[amount_of_slaves] = {0};
        communicator.recv(0, 0, gloop_state);
        do
        {
            boost::uint8_t is_inside_chunk[amount_of_slaves] = {0};
            communicator.recv(0, 0, player_coords);

            for (int unsigned chunk = 0; chunk != amount_of_slaves; chunk ++)
            {
                if (((player_coords.xdim + player_xdim_len) >= uni_chunks[current_rank_id][chunk].xdim_start || (player_coords.xdim >= uni_chunks[current_rank_id][chunk].xdim_start))&&
                (player_coords.xdim < (uni_chunks[current_rank_id][chunk].xdim_start + uni_chunk_xdim_len)) &&
                ((player_coords.ydim + player_ydim_len) >= uni_chunks[current_rank_id][chunk].ydim_start || (player_coords.ydim >= uni_chunks[current_rank_id][chunk].ydim_start)) &&
                (player_coords.ydim < (uni_chunks[current_rank_id][chunk].ydim_start + uni_chunk_ydim_len)))
                {
                    is_inside_chunk[chunk] = 1;
                    //rgba_colour_t tmp = {0, 0, 0};
                    // set all rgba colours to 255 aka white
                    memset(uni_chunks[current_rank_id][chunk].chunk_data, 255, uni_chunk_size*sizeof(rgba_colour_t));
                    player_coords.ydim -= (current_rank_id * uni_chunk_ydim_len);
                    //player_coords.xdim -= (current_rank_id * uni_chunk_xdim_len);

                    for (int unsigned y = player_coords.ydim; y != (player_coords.ydim + player_ydim_len); y++) {
                        if (y > (uni_chunks[current_rank_id][chunk].ydim_start + uni_chunk_ydim_len)) break;
                        for (int unsigned x = player_coords.xdim; x != (player_coords.xdim + player_xdim_len); x++) {
                            if (x > (uni_chunks[current_rank_id][chunk].xdim_start + uni_chunk_xdim_len)) break;
                            int unsigned pixpos = x + (y * uni_chunk_xdim_len);
                            uni_chunks[current_rank_id][chunk].chunk_data[pixpos] = {255, 0, 255 ,255};
                        }
    
                    }
                /*
                    for (int unsigned y = 0; y != uni_chunk_ydim_len; y++)
                    {
                        //if (y > (uni_chunks[current_rank_id][chunk].ydim_start + uni_chunk_ydim_len)) break;
                        for (int unsigned x = 0; x != uni_chunk_xdim_len; x++)
                        {
                            //if (x > (uni_chunks[current_rank_id][chunk].xdim_start + uni_chunk_xdim_len)) break;
                            if (y >= player_coords.ydim && y <= (player_coords.ydim + player_ydim_len))
                            {
                                if (x >= player_coords.xdim && x <= (player_coords.xdim + player_xdim_len)){
                                int unsigned pixpos = x + (y * uni_chunk_xdim_len);
                                uni_chunks[current_rank_id][chunk].chunk_data[pixpos] = {255, 0, 255 ,255};
                                }
                            }
                        }
                    }
*/
                }
            }
             
            communicator.send(0, 0, is_inside_chunk, amount_of_slaves);
 
            for (int unsigned chunk = 0; chunk != amount_of_slaves; chunk ++)
            {
                if (is_inside_chunk[chunk] == 1)
                {
                    communicator.send(0, 0, uni_chunks[current_rank_id][chunk].chunk_data, uni_chunk_size);
                }
            }
         

            //std::cout << "player x: " << player_coords.xdim << " y: " << player_coords.ydim  << std::endl;

            //if ((player_coords.xdim + player_xdim_len) > uni_chunks[0][0])
             
            communicator.recv(0, 0, gloop_state);

        } while (gloop_state == loop_state::__is_running);
    }

    return 0;
}
