import com.cycling74.max.*;

import java.util.ArrayList;


public class EventLooper extends com.cycling74.max.MaxObject {
    private long begin, end;  // start and end time of loop
    private boolean recording,
                    playing;
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
        recording = false;
        playing   = false;
        begin     = 0;
        end       = 0;
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

    /**
     * If recording stop recording, otherwise stop playing.
     */
    public void stop() {
        if (recording) {
            end = System.nanoTime();
            recording = false;
        } else {
            playing   = false;
        }
    }

    /**
     * Called when we receive a 'play' message.
     * If recording, stop and immediately begin looping,
     * else just start playing
     */
    public void play() {
        playing = true;
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
     * Loops events based on their timestamp.
     */
    public void _loop() {
        while (playing) {
            for (Event cur : buf) {
                try {
                    Thread.sleep(cur.time);
                } catch (java.lang.InterruptedException E) {
                    post("Thread interrupted");
                    E.printStackTrace();
                }

                // send out the current event
                outlet(0, cur.event);
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
