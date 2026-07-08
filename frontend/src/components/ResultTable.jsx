export default function ResultTable({
  columns,
  rows,
  darkMode,
}) {
  const theme = darkMode
    ? {
      table: "#1e1e1e",
      row1: "#1e1e1e",
      row2: "#252525",
      text: "#ffffff",
      border: "#333",
      header: "#2563eb",
    }
    : {
      table: "#ffffff",
      row1: "#ffffff",
      row2: "#f5f5f5",
      text: "#000000",
      border: "#ddd",
      header: "#2563eb",
    };
  if (
    !columns ||
    columns.length === 0
  ) {
    return null;
  }

  return (
    <div
      style={{
        marginTop: "20px",
        overflow: "hidden",
        borderRadius: "10px",
        border: `1px solid ${theme.border}`,
        background: theme.table,
        boxShadow: darkMode
          ? "0 4px 12px rgba(0,0,0,0.3)"
          : "0 4px 12px rgba(0,0,0,0.08)",
      }}
    >
      <div
        style={{
          overflowX: "auto",
        }}
      >
        <table
          style={{
            width: "100%",
            borderCollapse:
              "collapse",
            background: theme.table,
            color: theme.text,
            border: "none",
          }}
        >
          <thead>
            <tr>
              {columns.map(
                (column) => (
                  <th
                    key={column}
                    style={{
                      background: "#1d4ed8",
                      fontWeight: "600",
                      letterSpacing: "0.3px",
                      fontSize: "14px",
                      color: "#fff",
                      padding: "14px",
                      textAlign: "left",
                      fontWeight: "600",
                      position: "sticky",
                      top: 0,
                    }}
                  >
                    {column}
                  </th>
                )
              )}
            </tr>
          </thead>

          <tbody>
            {rows.map(
              (
                row,
                rowIndex
              ) => (
                <tr
                  key={rowIndex}
                  style={{
                    background:
                      rowIndex % 2 === 0
                        ? theme.row1
                        : theme.row2,
                    transition: "0.2s",
                  }}
                  onMouseEnter={(e) => {
                    e.currentTarget.style.background =
                      darkMode
                        ? "#333"
                        : "#e8f2ff";
                  }}
                  onMouseLeave={(e) => {
                    e.currentTarget.style.background =
                      rowIndex % 2 === 0
                        ? theme.row1
                        : theme.row2;
                  }}
                >
                  {row.map(
                    (
                      cell,
                      cellIndex
                    ) => (
                      <td
                        key={
                          cellIndex
                        }
                        style={{
                          padding: "14px 18px",
                          fontSize: "14px",
                          textAlign: isNaN(cell) ? "left" : "center",
                          border: `1px solid ${theme.border}`,
                        }}
                      >
                        {cell}
                      </td>
                    )
                  )}
                </tr>
              )
            )}
          </tbody>
        </table>
      </div>
    </div>
  );
}