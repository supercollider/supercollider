Module['makeDir'] = function(path) {
    // filter out empty splits (e.g. from leading slash)
    const parts = path.split('/').filter((x) => x);
    let current = '';
    for (const part of parts) {
        current += '/' + part;
        try {
            Module['FS'].mkdir(current);
        } catch (e) {
            if (e.errno !== 20 && e.code !== 'EEXIST') throw e;
        }
    }
}

/**
 * Adds a file (creating the directory if necessary) and writes its content
 * to the virtual filesystem.
 *
 * Accepts either a string (UTF-8 encoded) or a Uint8Array (written as-is).
 *
 * @param path {String} - absolute path, e.g. "/sc/myfile.sc"
 * @param content {String|Uint8Array} - file content
 */
Module['addFile'] = function(path, content) {
    const dir = path.slice(0, path.lastIndexOf('/'));
    Module['makeDir'](dir);

    Module['FS'].writeFile(path, content);
}

/**
 * Downloads a file from a URL into the virtual filesystem of sclang.
 * Throws error if download fails.
 *
 * @param url {String} - URL of the resource to download
 * @param path {String} - absolute destination path on the virtual filesystem,
 *   e.g. "/sc/myfile.zip"
 * @returns {Promise<void>} Throws error if download fails
 */
Module['downloadFile'] = async function(url, path) {
    const response = await fetch(url);
    if (!response.ok) {
        throw new Error(`Failed to download ${url}: ${response.statusText}`);
    }

    const buffer = new Uint8Array(await response.arrayBuffer());

    const dir = path.slice(0, path.lastIndexOf('/'));
    Module['makeDir'](dir);

    Module['FS'].writeFile(path, buffer);
}
