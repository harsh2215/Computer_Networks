// tcp-harsh.h
#ifndef TCP_HARSH_H
#define TCP_HARSH_H

#include "tcp-congestion-ops.h"
#include "ns3/nstime.h"
// #include "sequence-number.h"

namespace ns3 {

class TcpHarsh : public TcpCongestionOps
{
public:
  static TypeId GetTypeId (void);
  TcpHarsh ();
  virtual ~TcpHarsh ();

  virtual std::string GetName () const override;
  virtual Ptr<TcpCongestionOps> Fork () override;

  virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;
  virtual void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;


private:
  void HystartReset (Ptr<const TcpSocketState> tcb);
  void HystartUpdate (Ptr<TcpSocketState> tcb, const Time& delay);
  void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt) override;
  void CongestionStateSet(Ptr<TcpSocketState> tcb,const TcpSocketState::TcpCongState_t newState) override;
  Time HystartDelayThresh (const Time& t) const;
  SequenceNumber32 m_endSeq;

  // Hystart parameters and variables
  bool m_hystart;                              // Hystart toggle
  uint32_t m_hystartLowWindow;                 // Min cwnd to apply Hystart
  uint8_t m_hystartMinSamples;                 // Min delay samples for Hystart
  Time m_hystartAckDelta;                      // ACK spacing threshold
  Time m_hystartDelayMin;                      // Min delay threshold for Hystart
  Time m_hystartDelayMax;                      // Max delay threshold for Hystart
  Time m_delayMin;
  Time m_roundStart;                           // Start of Hystart round
  Time m_lastAck;                              // Time of last ACK
  Time m_currRtt;                              // Current RTT for Hystart
  uint8_t m_sampleCnt;                         // Sample count for delay measurements
  bool m_found;                                // Flag for exiting Slow Start
};

} // namespace ns3

#endif // TCP_HARSH_H
