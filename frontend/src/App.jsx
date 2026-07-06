import { useState } from "react";
import axios from "axios";
import ResultTable from "./components/ResultTable";
import SqlEditor from "./components/SqlEditor";
function App() {
  const [query, setQuery] = useState(
    "SELECT * FROM students;"
  );

  const [output, setOutput] =
    useState("");

  const [plan, setPlan] =
    useState([]);

  const [columns, setColumns] =
    useState([]);

  const [rows, setRows] =
    useState([]);

  const runQuery = async () => {
    try {
      const response =
        await axios.post(
          "http://127.0.0.1:5000/query",
          {
            sql: query,
          }
        );

      console.log(response.data);

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
        setOutput("");

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

        setOutput("");

        return;
      }

      setColumns([]);
      setRows([]);
      setPlan([]);

      setOutput(
        response.data.output
      );
    } catch (err) {
      console.error(err);

      setOutput(
        "Backend Error"
      );

      setPlan([]);
      setColumns([]);
      setRows([]);
    }
  };

  return (
    <div
      style={{
        padding: "20px",
        maxWidth: "1200px",
        margin: "0 auto",
      }}
    >
      <h1>miniSQL Studio</h1>

      <textarea
        rows="8"
        value={query}
        onChange={(e) =>
          setQuery(
            e.target.value
          )
        }
        style={{
          width: "100%",
          fontFamily:
            "monospace",
          fontSize: "14px",
        }}
      />

      <button
        onClick={runQuery}
        style={{
          marginTop: "15px",
          padding:
            "10px 20px",
        }}
      >
        Run Query
      </button>

      {plan.length > 0 && (
        <>
          <h2
            style={{
              marginTop: "30px",
            }}
          >
            Execution Plan
          </h2>

          <pre>
            {plan.join("\n")}
          </pre>
        </>
      )}

      {output && (
        <>
          <h2
            style={{
              marginTop: "30px",
            }}
          >
            Output
          </h2>

          <pre>{output}</pre>
        </>
      )}

      <ResultTable
        columns={columns}
        rows={rows}
      />
    </div>
  );
}

export default App;