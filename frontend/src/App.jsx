import { useState, useEffect, useRef } from "react";
import axios from "axios";
import ResultTable from "./components/ResultTable";

function App() {
  const [query, setQuery] = useState(
    "SELECT * FROM students;"
  );

  const [plan, setPlan] =
    useState([]);

  const [columns, setColumns] =
    useState([]);

  const [rows, setRows] =
    useState([]);

  const [error, setError] =
    useState("");

  const [message, setMessage] =
    useState("");

  const [history, setHistory] =
    useState([]);

  const [tables, setTables] =
    useState([]);

  const [darkMode, setDarkMode] =
    useState(true);

  const [loading, setLoading] = useState(false);

  const resultRef = useRef(null);

  const theme = darkMode
    ? {
      background: "#121212",
      panel: "#1e1e1e",
      border: "#333",
      text: "#ffffff",
      history: "#2a2a2a",
      button: "#2563eb",
    }
    : {
      background: "#ffffff",
      panel: "#f5f5f5",
      border: "#ddd",
      text: "#000000",
      history: "#f0f0f0",
      button: "#2563eb",
    };

  const buttonHover = {
    onMouseEnter: (e) => {
      e.currentTarget.style.transform =
        "scale(1.05)";
      e.currentTarget.style.boxShadow =
        "0 4px 12px rgba(0,0,0,0.3)";
    },

    onMouseLeave: (e) => {
      e.currentTarget.style.transform =
        "scale(1)";
      e.currentTarget.style.boxShadow =
        "none";
    },
  };

  const loadTables =
    async () => {
      try {
        const response =
          await axios.get(
            "http://127.0.0.1:5000/tables"
          );

        setTables(
          response.data
        );
      } catch (err) {
        console.error(err);
      }
    };

  useEffect(() => {
    loadTables();
  }, []);

  const scrollToResult = () => {
    setTimeout(() => {
      resultRef.current?.scrollIntoView({
        behavior: "smooth",
        block: "start",
      });
    }, 100);
  };

  const runQuery = async () => {
    setLoading(true);
    setHistory((prev) => {
      const filtered =
        prev.filter(
          (q) => q !== query
        );

      return [
        query,
        ...filtered,
      ].slice(0, 10);
    });


    try {
      const response =
        await axios.post(
          "http://127.0.0.1:5000/query",
          {
            sql: query,
          }
        );

      console.log(
        response.data
      );

      if (
        response.data.type ===
        "error"
      ) {
        setError(
          response.data.message
        );

        setMessage("");

        setColumns([]);
        setRows([]);

        scrollToResult();

        return;
      }

      if (
        response.data.type ===
        "message"
      ) {
        setMessage(
          response.data.message
        );

        setError("");

        setColumns([]);
        setRows([]);
        setPlan([]);

        scrollToResult();

        loadTables();

        return;
      }

      if (
        response.data.type ===
        "result"
      ) {
        setColumns(
          response.data.columns
        );

        setRows(
          response.data.rows
        );

        setPlan([]);

        setError("");
        setMessage("");

        scrollToResult();

        return;
      }

      if (
        response.data.type ===
        "plan"
      ) {
        setPlan(
          response.data.plan
        );

        setColumns([]);
        setRows([]);

        setError("");
        setMessage("");

        scrollToResult();

        return;
      }
    } catch (err) {
      console.error(err);

      setError(
        "Backend Error"
      );

      setMessage("");

      setColumns([]);
      setRows([]);
      setPlan([]);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div
      style={{
        display: "flex",
        gap: "20px",
        padding: "20px",
        minHeight: "100vh",
        background:
          theme.background,
        color: theme.text,
      }}
    >
      {/* Sidebar */}

      <div
        style={{
          width: "250px",
          display: "flex",
          flexDirection:
            "column",
          gap: "20px",
        }}
      >
        {/* Query History */}

        <div
          style={{
            background:
              theme.panel,
            border: `1px solid ${theme.border}`,
            padding: "15px",
            borderRadius:
              "8px",
          }}
        >
          <h3>
             Query History
          </h3>

          <p
            style={{
              fontSize: "12px",
              opacity: 0.6,
              marginBottom: "12px",
            }}
          >
            {history.length} recent quer{history.length === 1 ? "y" : "ies"}
          </p>

          {history.length ===
            0 ? (
            <p
              style={{
                opacity: 0.7,
                fontSize: "14px",
              }}
            >
              No queries yet
            </p>
          ) : (
            history.map(
              (
                item,
                index
              ) => (
                <div
                  key={index}
                  onClick={() =>
                    setQuery(
                      item
                    )
                  }
                  {...buttonHover}
                  style={{
                    cursor:
                      "pointer",
                    padding:
                      "8px",
                    marginBottom:
                      "8px",
                    background:
                      theme.history,
                    color:
                      theme.text,
                    borderRadius:
                      "4px",
                    fontSize:
                      "12px",
                    whiteSpace:
                      "pre-wrap",
                    transition:
                      "all 0.2s ease",
                  }}
                >
                  {item}
                </div>
              )
            )
          )}
          <br />
          <button
            onClick={() => {
              if (
                window.confirm(
                  "Clear all query history?"
                )
              ) {
                setHistory([]);
              }
            }}
            {...buttonHover}
            style={{
              width: "100%",
              padding: "8px",
              marginBottom: "12px",
              background: "#dc2626",
              color: "white",
              border: "none",
              borderRadius: "6px",
              cursor: "pointer",
              transition:
                "all 0.2s ease",
            }}
          >
            Clear History
          </button>
        </div>

        {/* Tables */}

        <div
          style={{
            background:
              theme.panel,
            border: `1px solid ${theme.border}`,
            padding: "15px",
            borderRadius:
              "8px",
          }}
        >
          <h3> Database Tables</h3>

          <p
            style={{
              fontSize: "12px",
              opacity: 0.6,
              marginBottom: "12px",
            }}
          >
            {tables.length} tables
          </p>

          {tables.map(
            (table) => (
              <div
                key={table}
                onClick={() =>
                  setQuery(
                    `SELECT * FROM ${table};`
                  )
                }
                {...buttonHover}
                style={{
                  padding:
                    "8px",
                  marginBottom:
                    "6px",
                  background:
                    theme.history,
                  borderRadius:
                    "4px",
                  cursor:
                    "pointer",
                  transition:
                    "all 0.2s ease",
                }}
              >
                {table}
              </div>
            )
          )}
        </div>
      </div>

      {/* Main Content */}

      <div
        style={{
          flex: 1,
        }}
      >
        <div
          style={{
            display: "flex",
            justifyContent: "space-between",
            alignItems: "center",
            marginBottom: "20px",
          }}
        >
          <div>
            <h1
              style={{
                margin: 0,
                color: "#2563eb",
                fontSize: "34px",
              }}
            >
               miniSQL Studio
            </h1>

            <p
              style={{
                marginTop: "6px",
                opacity: 0.7,
              }}
            >
              Lightweight SQL Query Engine
            </p>
          </div>

          <button
            onClick={() =>
              setDarkMode(!darkMode)
            }
            {...buttonHover}
            style={{
              padding: "10px 18px",
              borderRadius: "8px",
              border: "none",
              cursor: "pointer",
              fontWeight: "600",
              transition: "all 0.2s ease",
            }}
          >
            {darkMode
              ? "☀️ Light"
              : "🌙 Dark"}
          </button>
        </div>


        <div
          style={{
            background: theme.panel,
            border: `1px solid ${theme.border}`,
            borderRadius: "10px",
            padding: "20px",
            marginBottom: "25px",
          }}
        >
          <div
            style={{
              display: "flex",
              justifyContent: "space-between",
              alignItems: "center",
              marginBottom: "15px",
            }}
          >
            <h3
              style={{
                margin: 0,
              }}
            >
               SQL Editor
            </h3>

            <span
              style={{
                opacity: 0.7,
                fontSize: "13px",
              }}
            >
              Ctrl + Enter to Run
            </span>
          </div>
          <textarea
            rows="8"
            value={query}
            onChange={(e) =>
              setQuery(
                e.target.value
              )
            }
            onKeyDown={(e) => {
              if (e.ctrlKey && e.key === "Enter") {
                runQuery();
              }
            }}
            style={{
              width: "100%",
              height: "220px",
              fontFamily: "Consolas, monospace",
              fontSize: "15px",
              background: theme.background,
              color: theme.text,
              border: `1px solid ${theme.border}`,
              borderRadius: "8px",
              padding: "15px",
              resize: "vertical",
              outline: "none",
              lineHeight: "1.6",
              boxSizing: "border-box",
            }}
          />

          <button
            onClick={runQuery}
            disabled={loading}
            {...buttonHover}
            style={{
              marginTop: "18px",
              padding: "12px 26px",
              background: "#2563eb",
              color: "#fff",
              border: "none",
              borderRadius: "8px",
              cursor: "pointer",
              fontWeight: "600",
              fontSize: "15px",
              transition: "all 0.2s ease",
              opacity: loading ? 0.7 : 1,
              cursor: loading ? "not-allowed" : "pointer",
            }}
          >
            {loading
              ? "⏳ Executing..."
              : "▶ Execute Query"}
          </button>

        </div>


        <div ref={resultRef}>
          {error && (
            <div
              style={{
                background: "#fee2e2",
                color: "#b91c1c",
                padding: "16px",
                marginTop: "20px",
                borderRadius: "10px",
                border: "1px solid #ef4444",
                fontWeight: "500",
                whiteSpace: "pre-wrap",
              }}
            >
               {error}
            </div>
          )}

          {message && (
            <div
              style={{
                background: "#dcfce7",
                color: "#166534",
                padding: "16px",
                marginTop: "20px",
                borderRadius: "10px",
                border: "1px solid #22c55e",
                fontWeight: "500",
                whiteSpace: "pre-wrap",
              }}
            >
               {message}
            </div>
          )}

          {plan.length >
            0 && (
              <>
                <h2
                  style={{
                    marginTop:
                      "30px",
                  }}
                >
                  Execution
                  Plan
                </h2>

                <pre>
                  {plan.join(
                    "\n"
                  )}
                </pre>
              </>
            )}

          {columns.length > 0 && (
            <div
              style={{
                background: theme.panel,
                border: `1px solid ${theme.border}`,
                borderRadius: "10px",
                padding: "20px",
                marginTop: "25px",
              }}
            >
              <div
                style={{
                  display: "flex",
                  justifyContent: "space-between",
                  alignItems: "center",
                  marginBottom: "15px",
                }}
              >
                <h3
                  style={{
                    margin: 0,
                  }}
                >
                  📊 Query Result
                </h3>

                <span
                  style={{
                    opacity: 0.7,
                    fontSize: "13px",
                  }}
                >
                  {rows.length} row{rows.length !== 1 ? "s" : ""} • {columns.length} column{columns.length !== 1 ? "s" : ""}      </span>
              </div>

              <ResultTable
                columns={columns}
                rows={rows}
                darkMode={darkMode}
              />
            </div>
          )}

        </div>
        <div
          style={{
            marginTop: "40px",
            paddingTop: "20px",
            borderTop: `1px solid ${theme.border}`,
            display: "flex",
            justifyContent: "space-between",
            alignItems: "center",
            fontSize: "13px",
            opacity: 0.7,
          }}
        >
          <span> miniSQL Studio v1.0</span>

          <span>React • Flask • C++</span>
        </div>
      </div>
    </div>
  );
}

export default App;