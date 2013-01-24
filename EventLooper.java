import com.cycling74.max.*;

import java.lang.System;
import java.util.ArrayList;

public class EventLooper extends com.cycling74.max.MaxObject {
    private long begin, end;  // start and end time of loop
    private boolean recording;
    private ArrayList<Event> buf;  // buffer of events

    public class Event {
        protected Atom[] event;
        protected Long time;

        public Event(Atom[] e, Long t) {
            event = e;
            time = t;
        }
    }

    public EventLooper() {
        declareInlets(new int[]{DataTypes.ALL});
        declareOutlets(new int[]{DataTypes.ALL});

        buf   = new ArrayList<Event>();
        begin = 0;
        end   = 0;
    }

    /**
     * Called when we receive a 'record' message.
     * If we're not recording, begin recording.
     * Otherwise, stop recording and begin looping.
     */
    public void record() {
        begin = System.nanoTime();
        recording = true;
    }

    public void stop() {
        end = System.nanoTime();
        recording = false;
    }

    /**
     * Called when we receive a 'play' message.
     * If recording, stop and immediately begin looping,
     * else just start playing
     */
    public void play() {
        if (recording) {
            stop();
            _loop();
        } else
            _loop();
    }

    /**
     * Called after an initial recording is made.
     * Subtracts the timestamp of the first event from all events
     * in the sequence.
     */
    public void _trim() {
        Long time = buf.get(0).time;
        for (Event e : buf)
            e.time -= time;
    }

    /**
     * Actual method to loop playback.
     */
    public void _loop() {
        int len = buf.size();
        Event curEvent = buf.get(0);
        while (true) {
            try {
                Thread.sleep(40);
            } catch (java.lang.InterruptedException E) {
                post("Thread interrupted");
                E.printStackTrace();
            }
        }
    }

    /**
     * Records a list into the buffer if 'recording' is true,
     * Otherwise does nothing.
     */
    public void list(Atom[] args) {
        if (recording)
            buf.add(new Event(args, System.nanoTime()));
        else
            error("Not recording");
    }
}
