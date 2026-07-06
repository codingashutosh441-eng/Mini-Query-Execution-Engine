export default function ResultTable({
  columns,
  rows,
}) {
  if (
    !columns ||
    columns.length === 0
  ) {
    return null;
  }

  return (
    <table
      style={{
        width: "100%",
        borderCollapse:
          "collapse",
        marginTop: "20px",
      }}
    >
      <thead>
        <tr>
          {columns.map(
            (column) => (
              <th
                key={column}
                style={{
                  border:
                    "1px solid #ccc",
                  padding: "10px",
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
            <tr key={rowIndex}>
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
                      border:
                        "1px solid #ccc",
                      padding:
                        "10px",
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
  );
}