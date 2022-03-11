(() => {
    let quit = false;

    const processCommand = command => {
        if (quit) {
            return;
        }
        if (command === "exit" || command === "quit") {
            quit = true;
            PThread.terminateAllThreads();
            return;
        }
        Module.ccall("processCommand", "number", ["string"], [command]);
    };

    const listeners = new Set();
    Module.addMessageListener = l => { listeners.add(l); };
    Module.removeMessageListener = l => { listeners.delete(l); };
    Module.print = s => listeners.forEach(l => l(s));

    let queue = [];
    Module.postMessage = command => {
        if (queue !== null) {
            queue.push(command)
        } else {
            processCommand(command);
        }
    };
    Module.postRun = () => {
        queue.forEach(processCommand);
        queue = null;
    };
})();
