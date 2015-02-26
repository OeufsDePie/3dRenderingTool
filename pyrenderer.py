#!/usr/bin/python3
import subprocess
import time
import threading
import os

class PyRenderer(object):
    '''Allows to launch the 3D renderer from the command line

    '''

    EXEC = './renderer'

    def __init__(self):
        self._thread = None
        self._stop_lock = threading.Lock()
        self._stop_flag = False
        
    def open_renderer(self, ply_file):
        '''Launch the renderer on the given PLY file in a new thread and
        window.

        '''
        self._thread = threading.Thread(target=self._call_renderer,
                                        args=(ply_file,))
        self._thread.daemon = True # kill this thread when mother dies
        self._thread.start()
        
    def _call_renderer(self, ply_file):
        command = [PyRenderer.EXEC,
                   ply_file]
        # Create subprocess
        FNULL = open(os.devnull, 'w') # for silencing stdout
        sub = subprocess.Popen(command, stdout=FNULL)
        # Wait for interruption or completion
        print(sub.poll())
        while True:
            with self._stop_lock:
                result = sub.poll()
                if result:
                    print("Prog terminated")
                    self._stop_flag = False
                    return result
                if self._stop_flag:
                    print("Stop flag detected")
                    self._stop_flag = False
                    sub.terminate()
                    return result
            time.sleep(.1)
            

    def stop_renderer(self):
        '''Interrupts the rendering process and close the window
        '''
        if (self._thread.is_alive()):
            with self._stop_lock:
                print("Stop flag set")
                self._stop_flag = True

if __name__ == '__main__':
    pyrenderer = PyRenderer()
    pyrenderer.open_renderer('')
    time.sleep(2)
    pyrenderer.stop_renderer()
    time.sleep(2)
    pyrenderer.open_renderer('')
    input("Appuyer sur Entrée pour terminer")
