#ifndef SHARE_GC_G1_G1CONCURRENTBOTFIXING_HPP
#define SHARE_GC_G1_G1CONCURRENTBOTFIXING_HPP

#include "gc/g1/g1BOTUpdateCardSet.hpp"

#include "memory/iterator.hpp"
#include "utilities/globalDefinitions.hpp"
#include "utilities/ticks.hpp"

struct G1BOTUpdateStats {
  Ticks concurrent_phase_start_time;
};

class G1CollectedHeap;
class G1ConcurrentBOTUpdateThread;

class G1ConcurrentBOTUpdate: public CHeapObj<mtGC> {
  G1CollectedHeap* _g1h;

  volatile bool _in_progress;
  volatile bool _should_abort;
  // Workers.
  uint _n_workers;
  // Two counters to know when all workers have finished.
  uint _inactive_count; // Number of workers waiting for jobs
  uint _stopped_count; // Number of workers terminated
  G1ConcurrentBOTUpdateThread** _updater_threads;

  // The plab size recorded before evacuation.
  size_t _plab_word_size;

  // A flag to turn recording on/off. Mainly to disable recording for full gcs.
  bool _plab_recording_in_progress;

  // A list of card sets, each recording the cards (of plabs) that need to be updated.
  G1BOTUpdateCardSet* _card_sets;
  // A pointer into the list for job dispatching.
  G1BOTUpdateCardSet* _current;

  G1BOTUpdateStats _stats;

  void enlist_card_set(G1BOTUpdateCardSet* card_set);

  void update_bot_for_card_set(G1BOTUpdateCardSet* card_set);

public:
  G1ConcurrentBOTUpdate(G1CollectedHeap* g1h);

  bool in_progress() const { return _in_progress; }
  bool should_abort() const { return _should_abort; }

  // Signal the workers to concurrently process the card sets.
  void activate();
  // Abort the jobs and wait for workers to finish.
  void abort_and_wait();
  // Signal that the workers have all finished.
  void deactivate();
  // Workers use these to maintain _inactive_count/_stopped_count and notify possible waiters
  // waiting for the workers to finish.
  void note_active();
  void note_inactive();
  void note_stopped();
  // Terminate the threads.
  void stop();

  // Clear the card sets from previous gcs.
  void clear_card_sets();

  // Prepare BOT update with necessary information, e.g., plab size. Called before recording plabs.
  void pre_record_plab_allocation();

  // Record each plab allocation.
  void record_plab_allocation(HeapWord* plab_allocation, size_t word_size);

  // Setup for the concurrent phase after plab recording.
  void post_record_plab_allocation();

  // Entry point for the updater threads. Claim and process one of the card sets from the list.
  // Return whether there are possibly more. Return false if someone asked us to abort.
  bool update_bot_step();

  // Entry point for concurrent refinement threads or mutators that tries to do conc refinement.
  // These threads always have a specific card in mind, that is, the dirty card to refine.
  void update_bot_before_refine(HeapRegion* r, HeapWord* card_boundary);

  void threads_do(ThreadClosure* tc);
  void print_summary_info();
};

#endif // SHARE_GC_G1_G1CONCURRENTBOTFIXING_HPP
