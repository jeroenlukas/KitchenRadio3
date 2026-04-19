#ifndef KR_PROFILER_H
#define KR_PROFILER_H

#define MAX_PROFILES  10
#define MAX_RESULTS   50

class TimeProfile {
  private:
    uint32_t usStart;
    uint32_t results[MAX_RESULTS];
    uint8_t results_idx = 0;
    uint8_t results_cnt = 0;
  public:
    String name;

    TimeProfile(const char* name): name(name) {}

    void start()
    {
      usStart = micros();
    }

    void stop()
    {
      results[results_idx++] = micros() - usStart;
      if(results_idx == MAX_RESULTS) results_idx = 0;
      if(results_cnt < MAX_RESULTS) results_cnt++;  
    }

    String getResult()
    {      
      uint32_t cum = 0;
      uint32_t avg = 0;
      uint32_t low = 0xFFFFFFFF;
      uint32_t high = 0;
      
      // Iterate through results
      for(int i = 0; i < results_cnt; i++)
      {
        cum += results[i];
        if(results[i] > high) high = results[i];
        if(results[i] < low) low = results[i];
      }

      // Calculate the average
      avg = cum / results_cnt;


      return "[" + name + "] Average: " + String(avg)+ " us. Lowest: " + String(low) + " us. Highest: " + String(high) + " us (" + String(results_cnt) + " results)";
    }
};

class TimeProfiler {
  private:
    int itemCount = 0;
    TimeProfile* profiles[MAX_PROFILES];
  public:
    void add(TimeProfile* profile)
    {
      profiles[itemCount++] = profile;
    }

    void print()
    {
      Serial.println("--- TIME PROFILER ---");
      for(int i = 0; i < itemCount; i++)
      {
        Serial.println(profiles[i]->getResult());
      }
      Serial.println("---     (end)     ---");
    }
};

extern TimeProfiler profiler;

#endif