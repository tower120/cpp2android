package cpp2android.utils;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Thread-safe
 * get is fast, lock-less
 */
public class GrowOnlyArray<T> {
    private class Array{
        T memory[];
        final int capacity;
        int size = 0;
        final ReentrantLock update_lock = new ReentrantLock();

        @SuppressWarnings("unchecked")
        Array(int capacity){
            this.capacity = capacity;
            memory = (T[])new Object[capacity];
        }

        Array(int capacity, Array other){
            this.capacity = capacity;
            this.size = other.size;
            this.memory = Arrays.copyOf(other.memory, capacity);
        }
    }
    private AtomicReference<Array> array = new AtomicReference<Array>( new Array(32) );

    public int add(T obj){
        Array array = this.array.get();
        synchronized (array.update_lock){
            if (array.size >= array.capacity){
                Array newArray = new Array(array.capacity*2, array);
                this.array.set(newArray);
                array = newArray;
            }

            final int index = array.size;
            array.memory[index] = obj;

            array.size++;

            return index;
        }
    }

    public int size(){
        return array.get().size;
    }

    public T get(int index){
        return array.get().memory[index];
    }
}
