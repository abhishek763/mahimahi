/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef LOSS_QUEUE_HH
#define LOSS_QUEUE_HH

#include <queue>
#include <cstdint>
#include <string>
#include <random>

#include "file_descriptor.hh"

class LossQueue
{
private:
    std::queue<std::string> packet_queue_ {};

    virtual bool drop_packet( const std::string & packet ) = 0;

protected:
    std::default_random_engine prng_;

public:
    LossQueue();
    LossQueue(unsigned int seed);

    virtual ~LossQueue() {}

    void read_packet( const std::string & contents );

    void write_packets( FileDescriptor & fd );

    unsigned int wait_time( void );

    bool pending_output( void ) const { return not packet_queue_.empty(); }

    static bool finished( void ) { return false; }
};

class IIDLoss : public LossQueue
{
private:
    std::bernoulli_distribution drop_dist_;

    bool drop_packet( const std::string & packet ) override;

public:
    IIDLoss( const double loss_rate ) : drop_dist_( loss_rate ) {}
};

class SwitchingLink : public LossQueue
{
private:
    bool link_is_on_;
    std::exponential_distribution<> on_process_;
    std::exponential_distribution<> off_process_;

    uint64_t next_switch_time_;

    void calculate_next_switch_time( void );

    bool drop_packet( const std::string & packet ) override;

public:
    SwitchingLink( const double mean_on_time_, const double mean_off_time );

    unsigned int wait_time( void );
};

class GELoss : public LossQueue
{
private:
    bool drop_packet( const std::string & packet ) override;

    float p_gb_;
    float p_bg_;
    float p_lg_;
    float p_lb_;
    bool good_state_;
    std::uniform_real_distribution<> uniform_dis_;


public:
    GELoss(unsigned int seed, float p_gb, float p_bg, float p_lg, float p_lb) : LossQueue(seed), p_gb_(p_gb), p_bg_(p_bg), p_lg_(p_lg), p_lb_(p_lb), good_state_(true), uniform_dis_(0.0, 1.0) {}
};

#endif /* LOSS_QUEUE_HH */
