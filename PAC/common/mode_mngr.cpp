/// @file  modeMngr.cpp
/// @brief �������� ���������� ������� modeMngr.h.
/// 
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c USE_NO_COMB - ���������� ��� �������� (������� @ref g_greb).@n
/// @c DEBUG       - ���������� ���������� � ������� �������������� ���������� 
/// � �������.
/// 
/// @par ������� ������:
/// @$Rev: 1910 $.@n
/// @$Author: id@BMK $.@n
/// @$Date:: 2010-04-08 08:01:55#$.

#include "mode_mngr.h"
//-----------------------------------------------------------------------------
step_path::step_path(): start_time( 0 )
    {   
    }
//-----------------------------------------------------------------------------
int step_path::add_closed_dev( device *dev )
    {
    close_devices.push_back( dev );
    
    return 0;
    }
//-----------------------------------------------------------------------------
int step_path::add_opened_dev( device *dev )
    {
    open_devices.push_back( dev );
    
    return 0;
    }
//-----------------------------------------------------------------------------
int step_path::init()
    {
    start_time = get_millisec();
    return 0;
    }
//-----------------------------------------------------------------------------
int step_path::evaluate() const
    {   
    // ��������� �������� ������� ��������� (����� ��������� ����� ���������).
    for ( u_int i = 0; i < open_devices.size(); i++ )
        {
        open_devices[ i ]->on();
        }
    for ( u_int i = 0; i < close_devices.size(); i++ )
        {
        close_devices[ i ]->off();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int step_path::final()
    {

    for ( u_int i = 0; i < open_devices.size(); i++ )
        {
        open_devices[ i ]->off();
        }
    start_time = 0;

    return 0;
    }
//-----------------------------------------------------------------------------
void step_path::print() const
    {    
    if ( open_devices.size() )
        {
        Print( "      On [ %u ]:\n", open_devices.size() );
        for ( u_int i = 0; i < open_devices.size(); i++ )
            {
            Print( "\t" );
            open_devices[ i ]->print();            
            }        
        }

    if ( close_devices.size() )
        {
        Print( "      Off [ %u ]:\n", close_devices.size() );
        for ( u_int i = 0; i < close_devices.size(); i++ )
            {
            Print( "\t" );
            close_devices[ i ]->print();            
            }        
        }
    }
//-----------------------------------------------------------------------------
u_int_4 step_path::get_start_time() const
    {
    return start_time;
    }
//-----------------------------------------------------------------------------
void step_path::set_start_time( u_int_4 start_time ) 
    {
    this->start_time = start_time;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mode_manager::mode_manager( u_int_2 new_modes_cnt
                           ): par( 0 ),                           
                           step_duration_par_n( 0 ),
                           next_step_n( 0 ),
                           modes_cnt( new_modes_cnt ),
                           steps_cnt( 0 ),                           
                           steps( 0 ),
                           active_step( 0 )
    {
    if ( 0 == new_modes_cnt )
        {
#ifdef DEBUG
        Print( "Error! mode_manager constructor - new_modes_cnt = 0!\n" );
#endif // DEBUG
        return;    
        }
    step_duration_par_n = new u_char*[ new_modes_cnt ];
    next_step_n = new u_char*[ new_modes_cnt ];

    steps_cnt = new u_char[ new_modes_cnt ];
    active_step = new u_char[ new_modes_cnt ];
    memset( steps_cnt, 0, new_modes_cnt );
    memset( active_step, 0, new_modes_cnt );

    steps = new step_path*[ new_modes_cnt ];

    is_active_mode = new u_char[ new_modes_cnt ];
    memset( is_active_mode, 0, new_modes_cnt );

    for ( int i = 0; i < new_modes_cnt; i++ )
    	{        
        modes_devices.push_back( new step_path() );

        modes_start_time.push_back( 0 );
    	}
    modes_start_time.push_back( 0 );

    }
//-----------------------------------------------------------------------------
int mode_manager::init( u_int_2 mode, u_char start_step )
    {
    modes_devices.at( mode )->init();

    if ( 0 == steps_cnt[ mode ] )
        {
        return 0;
        }
    if ( check_correct_step_n( mode, start_step ) )
        {
        return -1;
        }

    active_step[ mode ] = start_step;  
    is_active_mode[ mode ] = 1;
    steps[ mode ][ active_step[ mode ] ].init(); 

#ifdef DEBUG
    Print( " INIT STEP [ %d ]\n", active_step[ mode ] );
    steps[ mode ][ active_step[ mode ] ].print();
    Print( " NEXT STEP -> %d \n", 
        next_step_n[ mode ][ active_step[ mode ] ] );        
#endif      

    modes_start_time.at( mode + 1 ) = get_millisec();

    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::final( u_int_2 mode )
    {
    modes_devices.at( mode )->final();

    if ( 0 == steps_cnt[ mode ] )
        {
        return 0;
        }
    if ( check_correct_step_n( mode, active_step[ mode ] ) )
        {
        return -1;
        }

#ifdef DEBUG
    Print( " FINAL ACTIVE STEP [ %d ] \n", active_step[ mode ] );
#endif 

    is_active_mode[ mode ] = 0;
    steps[ mode ][ active_step[ mode ] ].final();

    active_step[ mode ] = 0;

    modes_start_time.at( mode + 1 ) = get_millisec();
    modes_start_time.at( 0 ) = get_millisec();

    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::evaluate( u_int_2 mode )
    {
    modes_devices.at( mode )->evaluate();

    if ( 0 == steps_cnt[ mode ] )
        {
        return 0;
        }
    if ( check_correct_step_n( mode, active_step[ mode ] ) ) 
        {
        return -1;
        }
    u_char step_n = active_step[ mode ];
    steps[ mode ][ step_n ].evaluate();

    // [ 1 ] ���� �������� ������������ �����.
    // [ 2 ] ���� �������� ������������ ��� ����� ����.
    if ( par != 0 &&                                    // 1                                          
        step_duration_par_n[ mode ][ step_n ] != 0 &&   // 2       
        get_millisec() - steps[ mode ][ step_n ].get_start_time() > 
        par[ 0 ][ step_duration_par_n[ mode ][ step_n ] ] * 1000L &&
        next_step_n[ mode ][ step_n ] < 255 )
        {
        final( mode );  
        init( mode, next_step_n[ mode ][ step_n ] );

        evaluate( mode );
        }

    return 0;
    } 
//-----------------------------------------------------------------------------
int mode_manager::check_correct_step_n( u_int_2 mode, u_char step )
    {
    //�������� �� ���������� ����� ������.
    if ( mode >= modes_cnt )
        {
#ifdef DEBUG
        Print( "Error! mode_manager:: mode number >= modes count - \
               [%u] >= [%u]",
               mode, modes_cnt );
#endif // DEBUG
        return -2;    
        }

    //�������� �� ���������� ����� ����.
    if ( step >= steps_cnt[ mode ] )
        {
#ifdef DEBUG
        Print( "Error! mode_manager:: step number >= steps count - \
               [%u] >= [%u]",
               step, steps_cnt[ mode ] );
#endif // DEBUG
        return -1;    
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::set_param( saved_params_u_int_4 *par )
    {
    this->par = par;
    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::set_mode_config( u_int_2 mode, u_char new_steps_cnt )
    {
    if ( 0 == new_steps_cnt )
        {
#ifdef DEBUG
        Print( "ERROR! mode_manager::set_mode_config - new_steps_cnt[%d] = 0!", 
            new_steps_cnt );
#endif // DEBUG
        return -1;
        }

    // �������� �� ���������� ����� ������.
    if ( mode >= modes_cnt )
        {
#ifdef DEBUG
        Print( "Error! mode_manager::set_mode_config mode number >= modes \
               count - [%u] >= [%u]",
               mode, modes_cnt );
#endif // DEBUG
        return -2;    
        }

    steps_cnt[ mode ] = new_steps_cnt;

    steps[ mode ] = new step_path[ new_steps_cnt ];   
    step_duration_par_n[ mode ] = new u_char[ new_steps_cnt ];    
    next_step_n[ mode ] = new u_char[ new_steps_cnt ];

    for ( int i = 0; i < new_steps_cnt; i++ )
        {
        step_duration_par_n[ mode ][ i ] = 0;
        next_step_n[ mode ][ i ] = 0;
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::to_step( u_int_2 mode, u_char new_step )
    {
    if ( check_correct_step_n( mode, new_step - 1 ) ) return -1;

    final( mode );
    init( mode, new_step - 1 );
    evaluate( mode );

    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::add_closed_dev( u_int_2 mode, u_char step, device *dev )
    {
    if ( check_correct_step_n( mode, step ) ) 
        {
        return -1;
        }
    
    return steps[ mode ][ step ].add_closed_dev( dev );
    }
//-----------------------------------------------------------------------------
int mode_manager::add_opened_dev( u_int_2 mode, u_char step, device *dev )
    {
    if ( check_correct_step_n( mode, step ) ) 
        {
        return -1;
        }
    return steps[ mode ][ step ].add_opened_dev( dev );
    }
//-----------------------------------------------------------------------------
void mode_manager::print()
    {
    Print( "mode_manager[%u]\n", modes_cnt );
    for ( int j = 0; j < modes_cnt; j++ )
        {        
        if ( modes_devices.at( j )->close_devices.size() > 0 ||
             modes_devices.at( j )->open_devices.size() > 0 ||
             steps_cnt[ j ] > 0 )
            {
            Print( "  [%d]\n", j );            
            }

        modes_devices.at( j )->print();

        if ( steps_cnt[ j ] > 0 )
            {
            Print( "     steps count - %u \n", steps_cnt[ j ] );
            for ( int i = 0; i < steps_cnt[ j ]; i++ )
                {
                Print( "    [%2d], time step par[%d]; next step[%d]\n", 
                    i + 1, step_duration_par_n[ j ][ i ], next_step_n[ j ][ i ] );                                                
                steps[ j ][ i ].print();
                }

            Print( "\n" );
            }
        }             
    }
//-----------------------------------------------------------------------------
u_int mode_manager::get_active_step( u_int mode )
    {
    return 1 + active_step[ mode ];
    }
//-----------------------------------------------------------------------------
int mode_manager::is_current_step_evaluation_time_left( u_int_2 mode )
    {
    if ( 0 == steps_cnt[ mode ] )
        {
        return 0;
        }
    if ( check_correct_step_n( mode, active_step[ mode ] ) ) 
        {
        return -1;
        }
    u_char step_n = active_step[ mode ];

    // [ 1 ] ���� �������� ������������ �����.
    // [ 2 ] ���� �������� ������������ ��� ����� ����.
    if ( par != 0 &&                                    // 1                                     
        step_duration_par_n[ mode ][ step_n ] != 0 &&   // 2  
        get_millisec() - steps[ mode ][ step_n ].get_start_time() > 
        par[ 0 ][ step_duration_par_n[ mode ][ step_n ] ] * 1000L )
        {
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
unsigned long mode_manager::get_current_step_evaluation_time( u_int_2 mode )
    {
    if ( 0 == steps_cnt[ mode ] )
        {
        return 0;
        }
    if ( check_correct_step_n( mode, active_step[ mode ] ) ) 
        {
        return 0;
        }
    u_char step_n = active_step[ mode ];

    return get_millisec() - steps[ mode ][ step_n ].get_start_time();
    }
//-----------------------------------------------------------------------------
int mode_manager::add_mode_closed_dev( u_int_2 mode, device *dev )
    {
    if ( mode >= modes_cnt )
    	{
#ifdef DEBUG
        Print( "Error! mode_manager:: mode[ %u ] >= modes count[ %u ]!\n",
               mode, modes_cnt );
#endif // DEBUG
        return 2;
    	}   

    try
        {
        modes_devices.at( mode)->add_closed_dev( dev );
        }
    catch (...)
        {
        debug_break;
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int mode_manager::add_mode_opened_dev( u_int_2 mode, device *dev )
    {
    if ( mode >= modes_cnt )
        {
#ifdef DEBUG
        Print( "Error! mode_manager:: mode[ %u ] >= modes count[ %u ]!\n",
            mode, modes_cnt );
#endif // DEBUG
        return 2;
        }

    try
    	{
        modes_devices.at( mode)->add_opened_dev( dev );
    	}
    catch (...)
    	{
        debug_break;
    	return 1;
    	}
       
    return 0;
    }
//-----------------------------------------------------------------------------
unsigned long mode_manager::get_mode_evaluation_time( u_int mode )
    {
    if ( mode >= 0 && mode < modes_cnt )
        {
        return get_millisec() - modes_start_time.at( mode + 1 );
        }        

    return 0;
    }
//-----------------------------------------------------------------------------
unsigned long mode_manager::get_idle_time()
    {
    return get_millisec() - modes_start_time.at( 0 );
    }
//-----------------------------------------------------------------------------
