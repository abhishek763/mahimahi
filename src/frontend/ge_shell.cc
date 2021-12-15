/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <vector>
#include <string>
#include <iostream>

#include <getopt.h>

#include "loss_queue.hh"
#include "util.hh"
#include "ezio.hh"
#include "packetshell.cc"

using namespace std;

/*
(float) p_gb: probability of transitioning from good state to the bad state
(float) p_bg: probability of transitioning from bad state to the good state
(float) p_lg: probability of a loss in the good state
(float) p_lb: probability of a loss in the bad state
*/

void usage( const string & program_name )
{
    throw runtime_error( "Usage: " + program_name + " p_gb p_bg p_lg p_lb [COMMAND...]" );
}

int main( int argc, char *argv[] )
{
    try {
        /* clear environment while running as root */
        char ** const user_environment = environ;
        environ = nullptr;

        check_requirements( argc, argv );

        if ( argc < 5 ) {
            usage( argv[ 0 ] );
        }

        const double p_gb = myatof( argv[ 1 ] );
        if ( (0 <= p_gb && p_gb <= 1) ) {
            /* do nothing */
        } else {
            cerr << "Error: probability of transitioning from good state to the bad state (p_gb) must be between 0 and 1." << endl;
            usage( argv[ 0 ] );
        }

        const double p_bg = myatof( argv[ 2 ] );
        if ( (0 <= p_gb && p_gb <= 1) ) {
            /* do nothing */
        } else {
            cerr << "Error: probability of transitioning from bad state to the good state must be between 0 and 1." << endl;
            usage( argv[ 0 ] );
        }

        const double p_lg = myatof( argv[ 3 ] );
        if ( (0 <= p_lg && p_lg <= 1) ) {
            /* do nothing */
        } else {
            cerr << "Error: probability of a loss in the good state must be between 0 and 1." << endl;
            usage( argv[ 0 ] );
        }

        const double p_lb = myatof( argv[ 4 ] );
        if ( (0 <= p_lb && p_lb <= 1) ) {
            /* do nothing */
        } else {
            cerr << "Error: probability of a loss in the bad state must be between 0 and 1." << endl;
            usage( argv[ 0 ] );
        }


        vector<string> command;

        if ( argc == 5 ) {
            command.push_back( shell_path() );
        } else {
            for ( int i = 5; i < argc; i++ ) {
                command.push_back( argv[ i ] );
            }
        }

        PacketShell<GELoss> ge_loss_app( "ge_loss", user_environment );

        string shell_prefix = "[ge_loss] ";

        ge_loss_app.start_uplink( shell_prefix,
                                command,
                               p_gb, p_bg, p_lg, p_lb );
        ge_loss_app.start_downlink(0.0, 1.0, 0.0, 0.0 );

        return ge_loss_app.wait_for_exit();
    } catch ( const exception & e ) {
        print_exception( e );
        return EXIT_FAILURE;
    }
}
