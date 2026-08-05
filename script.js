 const API_BASE = "http://localhost:8080";

        function render(items, gridId) {
            const grid = document.getElementById(gridId);
            if (!items || !Array.isArray(items) || items.length === 0) {
                grid.innerHTML = `<div style="color:#334; padding:1rem;">No results found.</div>`;
                return;
            }
            grid.innerHTML = items.map(i => `
              <div class="card">
                <h3>${i.name} <small>${i.symbol}</small></h3>
                <div class="price">$${Number(i.price).toLocaleString()}</div>
                <div class="info">
                  <span>Change:</span>
                  <span class="${i.change >= 0 ? 'positive' : 'negative'}">${i.change >= 0 ? '+' : ''}${Number(i.change).toFixed(2)}%</span>
                </div>
                <div class="info">
                  <span>MCap:</span>
                  <span>$${Number(i.cap).toFixed(2)}B</span>
                </div>
                <div class="info">
                  <span>Category:</span>
                  <span>${i.cat ? i.cat.toUpperCase() : ""}</span>
                </div>
                <div class="score">Score: ${i.score}/100</div>
              </div>
            `).join('');
        }

        function renderRecommendations(items, containerId) {
            const container = document.getElementById(containerId);
            if (!items || !Array.isArray(items) || items.length === 0) {
                container.innerHTML = '';
                return;
            }
            container.innerHTML = `
                <div class="recommendation-header">
                    <h2>🌟 Top Recommendations for You</h2>
                    <p>Based on your preferences: High-scoring assets with strong market cap</p>
                </div>
                <div class="recommendation-grid">
                    ${items.map((i, idx) => `
                        <div class="recommendation-card">
                            <div class="recommendation-rank">#${idx + 1}</div>
                            <h3>${i.name} <small>${i.symbol}</small></h3>
                            <div class="recommendation-score">
                                <div class="score-circle">${i.score}</div>
                                <span>Recommendation Score</span>
                            </div>
                            <div class="recommendation-details">
                                <div class="detail-item">
                                    <span class="detail-label">Price:</span>
                                    <span class="detail-value">$${Number(i.price).toLocaleString()}</span>
                                </div>
                                <div class="detail-item">
                                    <span class="detail-label">24h Change:</span>
                                    <span class="detail-value ${i.change >= 0 ? 'positive' : 'negative'}">
                                        ${i.change >= 0 ? '+' : ''}${Number(i.change).toFixed(2)}%
                                    </span>
                                </div>
                                <div class="detail-item">
                                    <span class="detail-label">Market Cap:</span>
                                    <span class="detail-value">$${Number(i.cap).toFixed(2)}B</span>
                                </div>
                                <div class="detail-item">
                                    <span class="detail-label">Category:</span>
                                    <span class="detail-value category-badge">${i.cat ? i.cat.toUpperCase() : ""}</span>
                                </div>
                            </div>
                            <div class="recommendation-reason">
                                ${getRecommendationReason(i)}
                            </div>
                        </div>
                    `).join('')}
                </div>
            `;
        }

        function getRecommendationReason(item) {
            let reasons = [];
            if (item.score >= 90) reasons.push("Exceptional overall score");
            else if (item.score >= 85) reasons.push("High-quality asset");
            else if (item.score >= 80) reasons.push("Strong performance");
            
            if (item.cap > 100) reasons.push("Large market cap");
            else if (item.cap > 10) reasons.push("Established market presence");
            
            if (item.change > 5) reasons.push("Strong positive momentum");
            else if (item.change > 0) reasons.push("Positive trend");
            
            if (['defi', 'ai', 'tech'].includes(item.cat.toLowerCase())) {
                reasons.push("Preferred category");
            }
            
            return reasons.length > 0 ? `✓ ${reasons.join(' • ')}` : '✓ Recommended asset';
        }

        async function fetchAssets(type, query = "") {
            const response = await fetch(`${API_BASE}/api/search?q=${encodeURIComponent(query)}&type=${type}`);
            return response.json();
        }

        async function getRecommendations(type) {
            try {
                const response = await fetch(`${API_BASE}/api/recommend?type=${type}`);
                const items = await response.json();
                const containerId = type === 'crypto' ? 'cryptoRecommendations' : 'stockRecommendations';
                renderRecommendations(items, containerId);
            } catch (e) {
                console.error('Error fetching recommendations:', e);
            }
        }

        function showSection(section, tabButton) {
            document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            document.getElementById(section).classList.add('active');
            if (tabButton) tabButton.classList.add('active');

            if (section === 'crypto') {
                fetchAssets("crypto").then(items => render(items, 'gridCrypto'));
            } else if (section === 'stock') {
                fetchAssets("stock").then(items => render(items, 'gridStock'));
            } else if (section === 'dashboard') {
                loadStats();
            }
        }

        async function searchAndRender(type, query, gridId) {
            const items = await fetchAssets(type, query);
            render(items, gridId);
        }

        async function loadStats() {
            try {
                const response = await fetch(`${API_BASE}/api/stats`);
                const stats = await response.json();
                document.getElementById("totalAssets").innerText = stats.total || "-";
                document.getElementById("cryptoCount").innerText = stats.cryptos || "-";
                document.getElementById("stockCount").innerText = stats.stocks || "-";
                document.getElementById("avgScore").innerText = (stats.avgScore ? stats.avgScore + "%" : "-");
            } catch (e) {
                document.getElementById("totalAssets").innerText = "-";
                document.getElementById("cryptoCount").innerText = "-";
                document.getElementById("stockCount").innerText = "-";
                document.getElementById("avgScore").innerText = "-";
            }
        }

        document.getElementById('searchCrypto').addEventListener('input', e => {
            searchAndRender("crypto", e.target.value, 'gridCrypto');
        });
        document.getElementById('searchStock').addEventListener('input', e => {
            searchAndRender("stock", e.target.value, 'gridStock');
        });

        window.onload = function () {
            loadStats();
            fetchAssets("crypto").then(items => render(items, 'gridCrypto'));
            fetchAssets("stock").then(items => render(items, 'gridStock'));
        };
