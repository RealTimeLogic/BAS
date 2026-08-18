const API_ROOT = '/api';
const REQUEST_TIMEOUT_MS = 5000;
const DEVICE_UNAVAILABLE_MESSAGE =
  'Cannot reach the device API. Check that the server is running.';

export class ApiError extends Error {
  constructor(message, status = 0) {
    super(message);
    this.name = 'ApiError';
    this.status = status;
  }
}

function responseError(response, text) {
  if (response.status === 502 || response.status === 503 || response.status === 504) {
    return new ApiError(DEVICE_UNAVAILABLE_MESSAGE, response.status);
  }
  return new ApiError(text || `${response.status} ${response.statusText}`, response.status);
}

async function request(path, options = {}) {
  const controller = new AbortController();
  const timeout = window.setTimeout(() => controller.abort(), REQUEST_TIMEOUT_MS);

  try {
    const response = await fetch(`${API_ROOT}${path}`, {
      ...options,
      signal: controller.signal,
      headers: {
        Accept: 'application/json, text/plain;q=0.9',
        ...options.headers
      }
    });
    const text = await response.text();

    if (!response.ok) {
      throw responseError(response, text);
    }

    if (!text) return null;
    if ((response.headers.get('content-type') || '').includes('application/json')) {
      return JSON.parse(text);
    }
    return text;
  } catch (error) {
    if (error.name === 'AbortError') {
      throw new ApiError('The device did not respond within 5 seconds.');
    }
    if (error instanceof ApiError) throw error;
    throw new ApiError(DEVICE_UNAVAILABLE_MESSAGE);
  } finally {
    window.clearTimeout(timeout);
  }
}

async function streamRequest(path, onProgress) {
  const controller = new AbortController();
  let timeout;
  const resetTimeout = () => {
    window.clearTimeout(timeout);
    timeout = window.setTimeout(() => controller.abort(), REQUEST_TIMEOUT_MS);
  };

  resetTimeout();
  try {
    const response = await fetch(`${API_ROOT}${path}`, {
      signal: controller.signal,
      headers: { Accept: 'text/plain' }
    });

    if (!response.ok) {
      const text = await response.text();
      throw responseError(response, text);
    }

    if (!response.body) {
      const text = await response.text();
      if (onProgress) onProgress(text);
      return text;
    }

    const reader = response.body.getReader();
    const decoder = new TextDecoder();
    let text = '';

    while (true) {
      const { done, value } = await reader.read();
      if (done) {
        text += decoder.decode();
        if (onProgress) onProgress(text);
        return text;
      }

      resetTimeout();
      text += decoder.decode(value, { stream: true });
      if (onProgress) onProgress(text);
    }
  } catch (error) {
    if (error.name === 'AbortError') {
      throw new ApiError('The device stream was idle for 5 seconds.');
    }
    if (error instanceof ApiError) throw error;
    throw new ApiError(DEVICE_UNAVAILABLE_MESSAGE);
  } finally {
    window.clearTimeout(timeout);
  }
}

export const api = {
  health: () => request('/health'),
  deviceStatus: () => request('/device/status'),
  device: (id) => request(`/device/${encodeURIComponent(id)}`),
  setGpio: (pin, state) => request(`/gpio/${encodeURIComponent(pin)}/set/${state}`, {
    method: 'POST'
  }),
  inspect: (mode, deviceName) => request(`/inspect?mode=${encodeURIComponent(mode)}`, {
    headers: { 'X-Device': deviceName }
  }),
  latency: (milliseconds) => request(`/slow/${encodeURIComponent(milliseconds)}`),
  stream: (onProgress) => streamRequest('/stream', onProgress),
  checksum: (body) => request('/body', {
    method: 'POST',
    headers: { 'Content-Type': 'text/plain; charset=UTF-8' },
    body
  })
};
