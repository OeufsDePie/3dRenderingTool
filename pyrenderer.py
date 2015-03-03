#!/usr/bin/python3
import subprocess
import time
import threading
import os

class PyRenderer(object):
    '''Allows to launch the 3D renderer from the command line

    Arguments:
    exec_path   The path to the renderer executable
    '''

    def __init__(self, exec_path):
        self.exec_path = exec_path
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
        command = [self.exec_path,
                   ply_file]
        # Create subprocess
        FNULL = open(os.devnull, 'w') # for silencing stdout
        sub = subprocess.Popen(command, stdout=FNULL)
        # Wait for interruption or completion
        while True:
            with self._stop_lock:
                result = sub.poll()
                if result:
                    self._stop_flag = False
                    return result
                if self._stop_flag:
                    self._stop_flag = False
                    sub.terminate()
                    return result
            time.sleep(.1)
            

    def stop_renderer(self):
        '''Interrupts the rendering process and close the window
        '''
        if (self._thread.is_alive()):
            with self._stop_lock:
                self._stop_flag = True

if __name__ == '__main__':
    pyrenderer = PyRenderer('./renderer')
    print('Starting rendering...')
    pyrenderer.open_renderer('Resources/Castle_FinalColorized.ply')
    time.sleep(2)
    print('Stop rendering')
    pyrenderer.stop_renderer()
    time.sleep(2)
    print('Starting rendering again...')
    pyrenderer.open_renderer('Resources/Castle_FinalColorized.ply')
    input("Appuyer sur Entrée pour terminer")
