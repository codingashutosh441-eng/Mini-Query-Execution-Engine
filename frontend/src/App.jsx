import { useState, useEffect, useRef } from "react";
import axios from "axios";
import ResultTable from "./components/ResultTable";

function App() {

  // ==========================================================
  // STATE
  // ==========================================================

  const [query, setQuery] = useState(
    "SELECT * FROM students;"
  );

  const [plan, setPlan] = useState([]);

  const [columns, setColumns] = useState([]);

  const [rows, setRows] = useState([]);

  const [error, setError] = useState("");

  const [message, setMessage] = useState("");

  const [history, setHistory] = useState([]);

  const [tables, setTables] = useState([]);

  const [darkMode, setDarkMode] = useState(true);

  const [loading, setLoading] = useState(false);

  // Test Plan

  const [testResults, setTestResults] = useState([]);

  const [testRunning, setTestRunning] = useState(false);

  const [testSummary, setTestSummary] = useState(null);

  const [showTestCases, setShowTestCases] = useState(false);

  const resultRef = useRef(null);


  // ==========================================================
  // THEME
  // ==========================================================

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


  // ==========================================================
  // BUTTON HOVER
  // ==========================================================

  const buttonHover = {

    onMouseEnter: (e) => {
      e.currentTarget.style.transform = "scale(1.03)";
      e.currentTarget.style.boxShadow =
        "0 4px 12px rgba(0,0,0,0.3)";
    },

    onMouseLeave: (e) => {
      e.currentTarget.style.transform = "scale(1)";
      e.currentTarget.style.boxShadow = "none";
    },

  };


  // ==========================================================
  // LOAD TABLES
  // ==========================================================

  const loadTables = async () => {

    try {

      const response = await axios.get(
        "http://127.0.0.1:5000/tables"
      );

      setTables(response.data);

    } catch (err) {

      console.error(err);

    }

  };


  // ==========================================================
  // INITIAL LOAD
  // ==========================================================

  useEffect(() => {

    loadTables();

  }, []);


  // ==========================================================
  // SCROLL TO RESULT
  // ==========================================================

  const scrollToResult = () => {

    setTimeout(() => {

      resultRef.current?.scrollIntoView({
        behavior: "smooth",
        block: "start",
      });

    }, 100);

  };


  // ==========================================================
  // RUN NORMAL SQL QUERY
  // ==========================================================

  const runQuery = async () => {

    setLoading(true);

    // Add query to history

    setHistory((prev) => {

      const filtered = prev.filter(
        (q) => q !== query
      );

      return [
        query,
        ...filtered
      ].slice(0, 10);

    });


    try {

      const response = await axios.post(
        "http://127.0.0.1:5000/query",
        {
          sql: query,
        }
      );


      console.log(response.data);


      // ------------------------------------------------------
      // ERROR
      // ------------------------------------------------------

      if (
        response.data.type === "error"
      ) {

        setError(
          response.data.message
        );

        setMessage("");

        setColumns([]);

        setRows([]);

        setPlan([]);

        scrollToResult();

        return;
      }


      // ------------------------------------------------------
      // MESSAGE
      // ------------------------------------------------------

      if (
        response.data.type === "message"
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


      // ------------------------------------------------------
      // RESULT
      // ------------------------------------------------------

      if (
        response.data.type === "result"
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


      // ------------------------------------------------------
      // EXECUTION PLAN
      // ------------------------------------------------------

      if (
        response.data.type === "plan"
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

      setError("Backend Error");

      setMessage("");

      setColumns([]);

      setRows([]);

      setPlan([]);

    } finally {

      setLoading(false);

    }

  };


  // ==========================================================
  // RUN TEST PLAN
  // ==========================================================

  const runTestPlan = async () => {

    setTestRunning(true);

    setTestResults([]);

    setTestSummary(null);

    setShowTestCases(false);

    setError("");

    setMessage("");

    setColumns([]);

    setRows([]);

    setPlan([]);


    try {

      const response = await axios.post(
        "http://127.0.0.1:5000/test-plan"
      );


      console.log(
        "TEST PLAN:",
        response.data
      );


      setTestResults(
        response.data.tests
      );


      setTestSummary({

        total:
          response.data.total,

        passed:
          response.data.passed,

        failed:
          response.data.failed,

      });


      loadTables();


      setTimeout(() => {

        resultRef.current?.scrollIntoView({
          behavior: "smooth",
          block: "start",
        });

      }, 100);


    } catch (err) {

      console.error(err);

      setError(
        "Unable to run test plan. " +
        "Make sure the Flask backend is running."
      );

    } finally {

      setTestRunning(false);

    }

  };


  // ==========================================================
  // MAIN UI
  // ==========================================================

  return (

    <div
      style={{
        display: "flex",
        gap: "20px",
        padding: "20px",
        minHeight: "100vh",
        background: theme.background,
        color: theme.text,
        boxSizing: "border-box",
      }}
    >


      {/* =====================================================
          SIDEBAR
          ===================================================== */}

      <div
        style={{
          width: "250px",
          flexShrink: 0,
          display: "flex",
          flexDirection: "column",
          gap: "20px",
        }}
      >


        {/* ===================================================
            QUERY HISTORY
            =================================================== */}

        <div
          style={{
            background: theme.panel,
            border:
              `1px solid ${theme.border}`,
            padding: "15px",
            borderRadius: "8px",
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

            {history.length} recent quer
            {history.length === 1
              ? "y"
              : "ies"}

          </p>


          {history.length === 0 ? (

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
              (item, index) => (

                <div
                  key={index}

                  onClick={() =>
                    setQuery(item)
                  }

                  {...buttonHover}

                  style={{
                    cursor: "pointer",
                    padding: "8px",
                    marginBottom: "8px",
                    background:
                      theme.history,
                    color: theme.text,
                    borderRadius: "4px",
                    fontSize: "12px",
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


        {/* ===================================================
            DATABASE TABLES
            =================================================== */}

        <div
          style={{
            background: theme.panel,
            border:
              `1px solid ${theme.border}`,
            padding: "15px",
            borderRadius: "8px",
          }}
        >

          <h3>
            Database Tables
          </h3>


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
                  padding: "8px",
                  marginBottom: "6px",
                  background:
                    theme.history,
                  borderRadius: "4px",
                  cursor: "pointer",
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


      {/* =====================================================
          MAIN CONTENT
          ===================================================== */}

      <div
        style={{
          flex: 1,
          minWidth: 0,
        }}
      >


        {/* ===================================================
            HEADER
            =================================================== */}

        <div
          style={{
            display: "flex",
            justifyContent:
              "space-between",
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
              setDarkMode(
                !darkMode
              )
            }

            {...buttonHover}

            style={{
              padding: "10px 18px",
              borderRadius: "8px",
              border: "none",
              cursor: "pointer",
              fontWeight: "600",
              transition:
                "all 0.2s ease",
            }}
          >

            {darkMode
              ? "☀️ Light"
              : "🌙 Dark"}

          </button>

        </div>


        {/* ===================================================
            SQL EDITOR
            =================================================== */}

        <div
          style={{
            background: theme.panel,
            border:
              `1px solid ${theme.border}`,
            borderRadius: "10px",
            padding: "20px",
            marginBottom: "25px",
          }}
        >

          <div
            style={{
              display: "flex",
              justifyContent:
                "space-between",
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

              if (
                e.ctrlKey &&
                e.key === "Enter"
              ) {

                runQuery();

              }

            }}

            style={{
              width: "100%",
              height: "220px",
              fontFamily:
                "Consolas, monospace",
              fontSize: "15px",
              background:
                theme.background,
              color: theme.text,
              border:
                `1px solid ${theme.border}`,
              borderRadius: "8px",
              padding: "15px",
              resize: "vertical",
              outline: "none",
              lineHeight: "1.6",
              boxSizing:
                "border-box",
            }}

          />


          {/* =================================================
              BUTTONS
              ================================================= */}

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
              fontWeight: "600",
              fontSize: "15px",
              transition:
                "all 0.2s ease",
              opacity:
                loading ? 0.7 : 1,
              cursor:
                loading
                  ? "not-allowed"
                  : "pointer",
            }}
          >

            {loading
              ? "⏳ Executing..."
              : "▶ Execute Query"}

          </button>


          <button

            onClick={runTestPlan}

            disabled={
              testRunning ||
              loading
            }

            {...buttonHover}

            style={{
              marginTop: "18px",
              marginLeft: "10px",
              padding: "12px 26px",
              background: "#7c3aed",
              color: "#fff",
              border: "none",
              borderRadius: "8px",
              fontWeight: "600",
              fontSize: "15px",
              transition:
                "all 0.2s ease",
              opacity:
                testRunning
                  ? 0.7
                  : 1,
              cursor:
                testRunning
                  ? "not-allowed"
                  : "pointer",
            }}
          >

            {testRunning
              ? "⏳ Running Tests..."
              : "🧪 Run Test Plan"}

          </button>

        </div>


        {/* ===================================================
            RESULTS
            =================================================== */}

        <div ref={resultRef}>


          {/* =================================================
              TEST PLAN SUMMARY
              ================================================= */}

          {testResults.length > 0 && (

            <div
              style={{
                background:
                  theme.panel,
                border:
                  `1px solid ${theme.border}`,
                borderRadius:
                  "10px",
                padding:
                  "20px",
                marginTop:
                  "25px",
              }}
            >

              <div
                style={{
                  display:
                    "flex",
                  justifyContent:
                    "space-between",
                  alignItems:
                    "center",
                }}
              >

                <div>

                  <h2
                    style={{
                      margin: 0,
                      fontSize:
                        "20px",
                    }}
                  >

                    🧪 Test Plan

                  </h2>


                  <p
                    style={{
                      marginTop:
                        "8px",
                      marginBottom:
                        0,
                      opacity:
                        0.7,
                      fontSize:
                        "14px",
                    }}
                  >

                    {testSummary?.total}
                    {" "}
                    test cases executed

                  </p>

                </div>


                {/* SUMMARY */}

                {testSummary && (

                  <div
                    style={{
                      textAlign:
                        "right",
                    }}
                  >

                    <div
                      style={{
                        fontSize:
                          "20px",
                        fontWeight:
                          "700",
                      }}
                    >

                      {testSummary.passed}
                      /
                      {testSummary.total}
                      {" "}
                      Passed

                    </div>


                    <div
                      style={{
                        fontSize:
                          "13px",
                        marginTop:
                          "4px",
                        opacity:
                          0.7,
                      }}
                    >

                      {testSummary.failed === 0
                        ? "All tests passed"
                        : `${testSummary.failed} test${
                            testSummary.failed !== 1
                              ? "s"
                              : ""
                          } failed`
                      }

                    </div>

                  </div>

                )}

              </div>


              {/* =================================================
                  SUCCESS / FAILURE
                  ================================================= */}

              {testSummary && (

                <div
                  style={{
                    marginTop:
                      "18px",
                    padding:
                      "12px",
                    borderRadius:
                      "7px",
                    textAlign:
                      "center",
                    fontWeight:
                      "600",

                    background:
                      testSummary.failed === 0
                        ? (
                            darkMode
                              ? "#12351f"
                              : "#dcfce7"
                          )
                        : (
                            darkMode
                              ? "#3f1d1d"
                              : "#fee2e2"
                          ),

                    color:
                      testSummary.failed === 0
                        ? (
                            darkMode
                              ? "#86efac"
                              : "#166534"
                          )
                        : (
                            darkMode
                              ? "#fca5a5"
                              : "#b91c1c"
                          ),
                  }}
                >

                  {testSummary.failed === 0
                    ? "✓ All tests passed successfully"
                    : `✗ ${testSummary.failed} test${
                        testSummary.failed !== 1
                          ? "s"
                          : ""
                      } failed`
                  }

                </div>

              )}


              {/* =================================================
                  SHOW TEST CASES
                  ================================================= */}

              <button

                onClick={() =>
                  setShowTestCases(true)
                }

                {...buttonHover}

                style={{
                  width: "100%",
                  marginTop:
                    "14px",
                  padding:
                    "10px",
                  background:
                    "transparent",
                  color:
                    theme.text,
                  border:
                    `1px solid ${theme.border}`,
                  borderRadius:
                    "7px",
                  cursor:
                    "pointer",
                  fontWeight:
                    "600",
                  transition:
                    "all 0.2s ease",
                }}
              >

                👁 Show Test Cases

              </button>

            </div>

          )}


          {/* =================================================
              TEST CASE MODAL
              ================================================= */}

          {showTestCases && (

            <div

              onClick={(e) => {

                if (
                  e.target ===
                  e.currentTarget
                ) {

                  setShowTestCases(
                    false
                  );

                }

              }}

              style={{
                position:
                  "fixed",
                inset: 0,
                background:
                  "rgba(0, 0, 0, 0.7)",
                display:
                  "flex",
                alignItems:
                  "center",
                justifyContent:
                  "center",
                zIndex:
                  1000,
                padding:
                  "20px",
                boxSizing:
                  "border-box",
              }}
            >

              {/* =================================================
                  MODAL
                  ================================================= */}

              <div
                style={{
                  width:
                    "100%",
                  maxWidth:
                    "850px",
                  maxHeight:
                    "85vh",
                  overflowY:
                    "auto",
                  background:
                    theme.panel,
                  color:
                    theme.text,
                  border:
                    `1px solid ${theme.border}`,
                  borderRadius:
                    "12px",
                  padding:
                    "24px",
                  boxSizing:
                    "border-box",
                  boxShadow:
                    "0 20px 60px rgba(0,0,0,0.5)",
                }}
              >


                {/* ===============================================
                    MODAL HEADER
                    =============================================== */}

                <div
                  style={{
                    display:
                      "flex",
                    justifyContent:
                      "space-between",
                    alignItems:
                      "center",
                    paddingBottom:
                      "18px",
                    marginBottom:
                      "18px",
                    borderBottom:
                      `1px solid ${theme.border}`,
                  }}
                >

                  <div>

                    <h2
                      style={{
                        margin: 0,
                        fontSize:
                          "22px",
                      }}
                    >

                      🧪 Test Cases

                    </h2>


                    <p
                      style={{
                        margin:
                          "6px 0 0 0",
                        opacity:
                          0.65,
                        fontSize:
                          "13px",
                      }}
                    >

                      MiniSQL functionality
                      verification

                    </p>

                  </div>


                  <button

                    onClick={() =>
                      setShowTestCases(
                        false
                      )
                    }

                    style={{
                      width:
                        "36px",
                      height:
                        "36px",
                      display:
                        "flex",
                      alignItems:
                        "center",
                      justifyContent:
                        "center",
                      background:
                        theme.background,
                      border:
                        `1px solid ${theme.border}`,
                      color:
                        theme.text,
                      borderRadius:
                        "6px",
                      fontSize:
                        "22px",
                      cursor:
                        "pointer",
                    }}
                  >

                    ×

                  </button>

                </div>


                {/* ===============================================
                    TEST CASES
                    =============================================== */}

                <div>

                  {testResults.map(
                    (test, index) => (

                      <div
                        key={index}
                        style={{
                          border:
                            `1px solid ${theme.border}`,
                          borderRadius:
                            "8px",
                          marginBottom:
                            "10px",
                          background:
                            theme.background,
                          overflow:
                            "hidden",
                        }}
                      >

                        {/* =========================================
                            TEST HEADER
                            ========================================= */}

                        <div
                          style={{
                            display:
                              "grid",
                            gridTemplateColumns:
                              "1fr auto",
                            gap:
                              "20px",
                            alignItems:
                              "center",
                            padding:
                              "15px 16px",
                          }}
                        >

                          {/* LEFT */}

                          <div
                            style={{
                              minWidth:
                                0,
                            }}
                          >

                            <div
                              style={{
                                fontSize:
                                  "15px",
                                fontWeight:
                                  "600",
                                marginBottom:
                                  "8px",
                              }}
                            >

                              {index + 1}.
                              {" "}
                              {test.name}

                            </div>


                            {/* SQL */}

                            <div
                              style={{
                                fontFamily:
                                  "Consolas, monospace",
                                fontSize:
                                  "13px",
                                lineHeight:
                                  "1.5",
                                opacity:
                                  0.75,
                                wordBreak:
                                  "break-word",
                                overflowWrap:
                                  "anywhere",
                                background:
                                  darkMode
                                    ? "#181818"
                                    : "#f3f3f3",
                                padding:
                                  "9px 10px",
                                borderRadius:
                                  "5px",
                              }}
                            >

                              {test.sql}

                            </div>

                          </div>


                          {/* RIGHT - STATUS */}

                          <div
                            style={{
                              minWidth:
                                "80px",
                              textAlign:
                                "center",
                              padding:
                                "7px 12px",
                              borderRadius:
                                "6px",
                              fontSize:
                                "13px",
                              fontWeight:
                                "700",

                              background:
                                test.passed
                                  ? (
                                      darkMode
                                        ? "#12351f"
                                        : "#dcfce7"
                                    )
                                  : (
                                      darkMode
                                        ? "#3f1d1d"
                                        : "#fee2e2"
                                    ),

                              color:
                                test.passed
                                  ? (
                                      darkMode
                                        ? "#86efac"
                                        : "#166534"
                                    )
                                  : (
                                      darkMode
                                        ? "#fca5a5"
                                        : "#b91c1c"
                                    ),
                            }}
                          >

                            {test.passed
                              ? "✓ PASS"
                              : "✗ FAIL"}

                          </div>

                        </div>


                        {/* =========================================
                            FAILED OUTPUT
                            ========================================= */}

                        {!test.passed && (

                          <div
                            style={{
                              borderTop:
                                `1px solid ${theme.border}`,
                              padding:
                                "14px 16px",
                              background:
                                darkMode
                                  ? "#211616"
                                  : "#fff5f5",
                            }}
                          >

                            <div
                              style={{
                                fontSize:
                                  "13px",
                                fontWeight:
                                  "600",
                                marginBottom:
                                  "7px",
                              }}
                            >

                              Actual Output

                            </div>


                            <pre
                              style={{
                                margin:
                                  0,
                                padding:
                                  "10px",
                                background:
                                  darkMode
                                    ? "#181818"
                                    : "#ffffff",
                                border:
                                  `1px solid ${theme.border}`,
                                borderRadius:
                                  "5px",
                                whiteSpace:
                                  "pre-wrap",
                                wordBreak:
                                  "break-word",
                                fontFamily:
                                  "Consolas, monospace",
                                fontSize:
                                  "12px",
                                lineHeight:
                                  "1.5",
                                overflowX:
                                  "auto",
                              }}
                            >

                              {test.actual ||
                                "No output"}

                            </pre>

                          </div>

                        )}

                      </div>

                    )
                  )}

                </div>


                {/* ===============================================
                    MODAL SUMMARY
                    =============================================== */}

                {testSummary && (

                  <div
                    style={{
                      marginTop:
                        "18px",
                      padding:
                        "13px",
                      textAlign:
                        "center",
                      borderRadius:
                        "7px",
                      fontWeight:
                        "600",

                      background:
                        testSummary.failed === 0
                          ? (
                              darkMode
                                ? "#12351f"
                                : "#dcfce7"
                            )
                          : (
                              darkMode
                                ? "#3f1d1d"
                                : "#fee2e2"
                            ),

                      color:
                        testSummary.failed === 0
                          ? (
                              darkMode
                                ? "#86efac"
                                : "#166534"
                            )
                          : (
                              darkMode
                                ? "#fca5a5"
                                : "#b91c1c"
                            ),
                    }}
                  >

                    {testSummary.passed}
                    /
                    {testSummary.total}
                    {" "}
                    tests passed

                  </div>

                )}


                {/* ===============================================
                    CLOSE BUTTON
                    =============================================== */}

                <button

                  onClick={() =>
                    setShowTestCases(
                      false
                    )
                  }

                  {...buttonHover}

                  style={{
                    width:
                      "100%",
                    marginTop:
                      "14px",
                    padding:
                      "11px",
                    background:
                      "#2563eb",
                    color:
                      "#ffffff",
                    border:
                      "none",
                    borderRadius:
                      "7px",
                    cursor:
                      "pointer",
                    fontWeight:
                      "600",
                    fontSize:
                      "14px",
                  }}
                >

                  Close

                </button>

              </div>

            </div>

          )}


          {/* =================================================
              ERROR
              ================================================= */}

          {error && (

            <div
              style={{
                background:
                  "#fee2e2",
                color:
                  "#b91c1c",
                padding:
                  "16px",
                marginTop:
                  "20px",
                borderRadius:
                  "10px",
                border:
                  "1px solid #ef4444",
                fontWeight:
                  "500",
                whiteSpace:
                  "pre-wrap",
              }}
            >

              {error}

            </div>

          )}


          {/* =================================================
              MESSAGE
              ================================================= */}

          {message && (

            <div
              style={{
                background:
                  "#dcfce7",
                color:
                  "#166534",
                padding:
                  "16px",
                marginTop:
                  "20px",
                borderRadius:
                  "10px",
                border:
                  "1px solid #22c55e",
                fontWeight:
                  "500",
                whiteSpace:
                  "pre-wrap",
              }}
            >

              {message}

            </div>

          )}


          {/* =================================================
              EXECUTION PLAN
              ================================================= */}

          {plan.length > 0 && (

            <>

              <h2
                style={{
                  marginTop:
                    "30px",
                }}
              >

                Execution Plan

              </h2>


              <pre>

                {plan.join("\n")}

              </pre>

            </>

          )}


          {/* =================================================
              QUERY RESULT
              ================================================= */}

          {columns.length > 0 && (

            <div
              style={{
                background:
                  theme.panel,
                border:
                  `1px solid ${theme.border}`,
                borderRadius:
                  "10px",
                padding:
                  "20px",
                marginTop:
                  "25px",
              }}
            >

              <div
                style={{
                  display:
                    "flex",
                  justifyContent:
                    "space-between",
                  alignItems:
                    "center",
                  marginBottom:
                    "15px",
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
                    opacity:
                      0.7,
                    fontSize:
                      "13px",
                  }}
                >

                  {rows.length}
                  {" "}
                  row
                  {rows.length !== 1
                    ? "s"
                    : ""}
                  {" • "}
                  {columns.length}
                  {" "}
                  column
                  {columns.length !== 1
                    ? "s"
                    : ""}

                </span>

              </div>


              <ResultTable
                columns={columns}
                rows={rows}
                darkMode={
                  darkMode
                }
              />

            </div>

          )}

        </div>


        {/* ===================================================
            FOOTER
            =================================================== */}

        <div
          style={{
            marginTop:
              "40px",
            paddingTop:
              "20px",
            borderTop:
              `1px solid ${theme.border}`,
            display:
              "flex",
            justifyContent:
              "space-between",
            alignItems:
              "center",
            fontSize:
              "13px",
            opacity:
              0.7,
          }}
        >

          <span>
            miniSQL Studio v1.0
          </span>

          <span>
            React • Flask • C++
          </span>

        </div>

      </div>

    </div>

  );

}


export default App;